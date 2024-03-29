/***********************************************************************

This file is part of the GilViewer project source files.

GilViewer is an open source 2D viewer (raster and vector) based on Boost
GIL and wxWidgets.


Homepage:

        http://code.google.com/p/gilviewer

Copyright:

        Institut Geographique National (2009)

Authors:

        Olivier Tournaire, Adrien Chauve




    GilViewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    GilViewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with GilViewer.  If not, see <http://www.gnu.org/licenses/>.

***********************************************************************/

#include <iostream>
#include <sstream>
#include <limits>
#include <utility>

#include <boost/filesystem.hpp>

#include <boost/gil/algorithm.hpp>
#include "boost/gil/extension/numeric/sampler.hpp"
#include "boost/gil/extension/numeric/resample.hpp"

#include <wx/dc.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/log.h>
#include <wx/config.h>

#include "../tools/orientation_2d.hpp"
#include "../tools/color_lookup_table.hpp"
#include "../layers/image_types.hpp"
#include "../gui/image_layer_settings_control.hpp"

#include "image_layer.hpp"
#include "image_layer_screen_image_functor.hpp"
#include "image_layer_min_max_functor.hpp"
#include "image_layer_histogram_functor.hpp"
#include "image_layer_to_string_functor.hpp"
#include "image_layer_infos_functor.hpp"


class alpha_image_type : public boost::gil::gray8_image_t {};

using namespace std;
using namespace boost::gil;
using namespace boost;

unsigned int image_layer::m_gamma_array_size = 1000;

#include <boost/variant/static_visitor.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/bind.hpp>

struct min_max_visitor : public boost::static_visitor< pair<float, float> >
{
    template <typename ViewType>
            result_type operator()(const ViewType& v) const { return apply_operation(v, any_view_min_max()); }
};

struct type_channel_visitor : public boost::static_visitor<string>
{
    template <typename ViewType>
            result_type operator()(const ViewType& v) const { return apply_operation(v, type_channel_functor()); }
};

struct nb_components_visitor : public boost::static_visitor<unsigned int>
{
    template <typename ViewType>
            result_type operator()(const ViewType& v) const { return apply_operation(v, nb_components_functor()); }
};

struct histogram_visitor : public boost::static_visitor<shared_ptr<const histogram_functor::histogram_type> >
{
    histogram_visitor(double &min, double &max) : m_min(min), m_max(max) {}

    template <typename ViewType>
            result_type operator()(const ViewType& v) const { return apply_operation(v, histogram_functor(m_min, m_max)); }

private:
    double m_min, m_max;
};

struct width_visitor : public boost::static_visitor<int>
{
    template <typename ViewType>
            result_type operator()(const ViewType& v) const { return v.width(); }
};

struct height_visitor : public boost::static_visitor<int>
{
    template <typename ViewType>
            result_type operator()(const ViewType& v) const { return v.height(); }
};

struct image_position_to_string_visitor : public boost::static_visitor<>
{
    image_position_to_string_visitor(const int i, const int j, std::ostringstream& oss) : m_i(i), m_j(j), m_oss(oss) {}

    template <typename ViewType>
            result_type operator()(const ViewType& v) const { return apply_operation(v, any_view_image_position_to_string_functor(m_i, m_j, m_oss)); }

private:
    const int m_i, m_j;
    ostringstream& m_oss;
};

struct screen_image_visitor : public boost::static_visitor<>
{
    screen_image_visitor(boost::gil::dev3n8_view_t &screen_view,
                         channel_converter_functor cc,
                         const layer_transform& trans,
                         boost::gil::gray8_view_t& canal_alpha,
                         const double min_alpha,
                         const double max_alpha,
                         const unsigned char alpha,
                         bool isTransparent) : m_screen_view(screen_view),
    m_cc(cc),
    m_transform(trans),
    m_canal_alpha(canal_alpha),
    m_min_alpha(min_alpha),
    m_max_alpha(max_alpha),
    m_alpha(alpha),
    m_is_transparent(isTransparent) {}

    template <typename ViewType>
            result_type operator()(const ViewType& v) const { return apply_operation(v, screen_image_functor(m_screen_view, m_cc, m_transform, m_canal_alpha, m_min_alpha, m_max_alpha, m_alpha, m_is_transparent)); }

private:
    boost::gil::dev3n8_view_t &m_screen_view;
    channel_converter_functor m_cc;
    layer_transform m_transform;
    boost::gil::gray8_view_t& m_canal_alpha;
    const double m_min_alpha;
    const double m_max_alpha;
    const unsigned char m_alpha;
    bool m_is_transparent;
};

struct subimage_visitor : public boost::static_visitor<image_layer::variant_view_t::type>
{
    subimage_visitor(int xMin, int yMin, int width, int height) : m_xmin(xMin), m_ymin(yMin), m_width(width), m_height(height) {}

    template <typename ViewType>
            result_type operator()(const ViewType& v) const { return subimage_view(v, m_xmin, m_ymin, m_width, m_height); }
private:
    int m_xmin, m_ymin, m_width, m_height;
};

void image_layer::init()
{
    min_max_visitor mmv;
    m_minmaxResult = apply_visitor( mmv, m_variant_view->value );
    intensity_min(m_minmaxResult.first);
    intensity_max(m_minmaxResult.second);

    alpha(255);
    /*
    intensity_min(0.);
    intensity_max(255.);
    */
    transparent(false);
    transparency_max(0.);
    transparency_min(0.);
    gamma(1.);

    m_cLUT = boost::shared_ptr<color_lookup_table>(new color_lookup_table);

    channels(0,1,2);
    alpha_channel(false,0);
}

image_layer::image_layer(const image_ptr &image, const std::string &name_, const std::string &filename_, const variant_view_ptr& v):
        layer(),
        m_img(image),
        m_variant_view(v),
        m_gamma_array( shared_array<float>(new float[m_gamma_array_size+1]) )
{
    if(!v)
    {
        m_variant_view.reset( new variant_view_t( boost::gil::view(m_img->value) ) );
    }

    name(name_);
    filename(filename_);

    init();
}

layer::ptrLayerType image_layer::create_image_layer(const image_ptr &image, const std::string &name, const std::string &filename, const variant_view_ptr& v)
{
    return ptrLayerType(new image_layer(image,name,filename,v));
}

void image_layer::update(int width, int height)
{
    // Lecture de la configuration des differentes options ...
    wxConfigBase *pConfig = wxConfigBase::Get();
    if (pConfig == NULL)
        return;

    bool loadWholeImage=false, bilinearZoom=false;

    pConfig->Read(wxT("/Options/LoadWoleImage"), &loadWholeImage, true); //TODO
    pConfig->Read(wxT("/Options/BilinearZoom"), &bilinearZoom, false);

    dev3n8_image_t screen_image(width, height);
    dev3n8_view_t screen_view = boost::gil::view(screen_image);

    if(!m_alpha_img) m_alpha_img.reset(new alpha_image_t);
    m_alpha_img->recreate(screen_view.dimensions());
    alpha_image_t::view_t alpha_view = boost::gil::view(*m_alpha_img);
    fill_pixels(alpha_view, 0);

    unsigned int nb_channels = static_cast<int>(nb_components());
    if(m_red>=nb_channels)
        m_red=nb_channels-1;
    if(m_green>=nb_channels)
        m_green=nb_channels-1;
    if(m_blue>=nb_channels)
        m_blue=nb_channels-1;
    channel_converter_functor my_cc(
            intensity_min(), intensity_max(),
            m_gamma_array, m_gamma_array_size,
            *m_cLUT, m_red, m_green, m_blue);
    screen_image_visitor siv(screen_view, my_cc, transform(), alpha_view, m_transparencyMin, m_transparencyMax, m_alpha, transparent());
    apply_visitor( siv, m_variant_view->value );

    wxImage monImage(screen_view.width(), screen_view.height(), interleaved_view_get_raw_data(screen_view), true);
    monImage.SetAlpha(interleaved_view_get_raw_data(boost::gil::view(*m_alpha_img)), true);

    m_bitmap = boost::shared_ptr<wxBitmap>(new wxBitmap(monImage));
}

void image_layer::draw(wxDC &dc, wxCoord x, wxCoord y, bool transparent) const
{
    dc.DrawBitmap(*m_bitmap, x, y, transparent); //-m_translationX+x*m_zoomFactor, -m_translationX+y*m_zoomFactor
}

unsigned int image_layer::nb_components() const
{
    //return apply_operation(m_view->value, nb_components_functor());
    return apply_visitor( nb_components_visitor(), m_variant_view->value );
}

string image_layer::type_channel() const
{
    // return apply_operation(m_view->value, type_channel_functor());
    return apply_visitor( type_channel_visitor(), m_variant_view->value );
}

shared_ptr<const layer::histogram_type> image_layer::histogram(double &min, double &max) const
{
    min = m_minmaxResult.first;
    max = m_minmaxResult.second;
    histogram_visitor hv(min, max);
    return apply_visitor(hv, m_variant_view->value);
}

string image_layer::pixel_value(const wxRealPoint& p) const
{
    ostringstream oss;
    oss.precision(6);
    oss<<"(";
    wxPoint pt=transform().to_local_int(p);
    image_position_to_string_visitor iptsv(pt.x, pt.y, oss);
    apply_visitor( iptsv, m_variant_view->value );
    oss<<")";
    return oss.str();
}

boost::shared_ptr<color_lookup_table> image_layer::colorlookuptable()
{
    return m_cLUT;
}

void image_layer::orientation(const boost::shared_ptr<orientation_2d> &orientation)
{
    m_ori->origin_x( orientation->origin_x() );
    m_ori->origin_y( orientation->origin_y() );
    m_ori->step( orientation->step() );
    m_ori->zone_carto( orientation->zone_carto() );
    m_ori->size_x( orientation->size_x() );
    m_ori->size_y( orientation->size_y() );
}

void image_layer::gamma(double gamma)
{
    // Then compute the gamma table (if needed)
    if (gamma != m_gamma)
    {
        m_gamma = 1. / gamma;
        for (unsigned int i=0; i<= m_gamma_array_size; ++i)
            m_gamma_array[i] = std::pow(((double) i)/ m_gamma_array_size, m_gamma);
        m_gamma = gamma;
    }
}


layer::ptrLayerType image_layer::crop_local(const wxRealPoint& p0, const wxRealPoint& p1) const
{
    // compute local coordinates
    wxRealPoint q0=p0;
    wxRealPoint q1=p1;

    //  q0 = min point, q1 = max point
    if(q0.x>q1.x) std::swap(q0.x,q1.x);
    if(q0.y>q1.y) std::swap(q0.y,q1.y);

    // clip to image range
    int w = width();
    int h = height();
    if(q0.x<0) q0.x=0;
    if(q0.y<0) q0.y=0;
    if(q1.x>w) q1.x=w;
    if(q1.y>h) q1.y=h;

    // get integral image coordinates
    q0.x = floor(q0.x+0.5);
    q0.y = floor(q0.y+0.5);
    q1.x = floor(q1.x+0.5);
    q1.y = floor(q1.y+0.5);

    int x0 = (int)(q0.x);
    int y0 = (int)(q0.y);
    int w0  = (int)(q1.x)-x0;
    int h0  = (int)(q1.y)-y0;

    // abort if trivial range
    if(w0<=0 || h0<=0) return ptrLayerType();
    
    subimage_visitor sv(x0, y0, w0, h0);
    variant_view_t::type crop = apply_visitor( sv, m_variant_view->value );
    //view_ptr crop_ptr(new view_t(crop));
    variant_view_ptr crop_ptr(new variant_view_t(crop));
    boost::filesystem::path file(boost::filesystem::system_complete(filename()));
    std::ostringstream oss;
    oss << ".crop" <<x0<<"_"<<y0<<"_"<<w0<<"x"<<h0;
#if BOOST_FILESYSTEM_VERSION > 2
    file.replace_extension(oss.str() + file.extension().string());
#else
    file.replace_extension(oss.str() + file.extension());
#endif
    std::string name = file.string();

    image_layer *l = new image_layer(m_img, name, file.string(), crop_ptr);

    // fix "off by 1 pixel transform" errors for rotated images
    q1.x -= 1;
    q1.y -= 1;

    wxRealPoint r0 = transform().from_local(q0);
    wxRealPoint r1 = transform().from_local(q1);
    if(r0.x>r1.x) std::swap(r0.x,r1.x);
    if(r0.y>r1.y) std::swap(r0.y,r1.y);

    l->transform() = transform();
    l->transform().translation_x(0);
    l->transform().translation_y(0);
    l->transform().translate(r0);
    l->transform().orientation(transform().orientation(),w0,h0);

    // todo : handle Orientation2D of if it exists ... ??

    return ptrLayerType(l);
}

vector<string> image_layer::available_formats_extensions() const
{
    vector<string> extensions;
    extensions.push_back("tif");
    extensions.push_back("TIF");
    extensions.push_back("tiff");
    extensions.push_back("TIFF");
    extensions.push_back("jpg");
    extensions.push_back("JPG");
    extensions.push_back("jpeg");
    extensions.push_back("JPEG");
    extensions.push_back("png");
    extensions.push_back("PNG");
    return extensions;
}

string image_layer::available_formats_wildcard() const
{
    ostringstream wildcard;
    wildcard << "All supported image files (*.tif;*.tiff;*.png;*.jpg;*.jpeg)|*.tif;*.tiff;*.TIF;*.TIFF;*.png;*.PNG;*.jpg;*.jpeg;*.JPG;*.JPEG|";
    wildcard << "TIFF (*.tif;*.tiff)|*.tif;*.tiff;*.TIF;*.TIFF|";
    wildcard << "PNG (*.png)|*.png;*.PNG|";
    wildcard << "JPEG (*.jpg;*.jpeg)|*.jpg;*.jpeg;*.JPG;*.JPEG|";
    return wildcard.str();
}

layer_settings_control* image_layer::build_layer_settings_control(unsigned int index, layer_control* parent)
{
    return new image_layer_settings_control(index, parent);
}

double image_layer::center_x() {return 0.5*width ();}
double image_layer::center_y() {return 0.5*height();}

bool image_layer::snap( eSNAP snap, double d2[], const wxRealPoint& p, wxRealPoint& psnap )
{

    if(!(snap & SNAP_GRID)) return false;

    wxRealPoint q = transform().to_local(p);
    int h=height();
    int w=width();
    if( q.x < -0.5 || q.y < -0.5 || q.x > w+0.5 || q.y > h+0.5 ) return false;

    wxRealPoint qsnap (floor(q.x+0.5),floor(q.y+0.5));

    wxRealPoint v(q-qsnap);
    double d = v.x*v.x+v.y*v.y;
    if(d>=d2[SNAP_GRID]) return false;

    for(unsigned int i=0; i<SNAP_GRID;++i) d2[i]=0;
    d2[SNAP_GRID]=d;
    psnap = transform().from_local(qsnap);
    return true;
}

unsigned int image_layer::width () const {return apply_visitor(  width_visitor(), m_variant_view->value );}
unsigned int image_layer::height() const {return apply_visitor( height_visitor(), m_variant_view->value );}
    
