#include "../config/config.hpp"
#if GILVIEWER_USE_GDAL

#include "gilviewer_file_io_shp.hpp"

#include <boost/filesystem/convenience.hpp>
#include <boost/variant/get.hpp>

#include <gdal/ogrsf_frmts.h>

#include "gilviewer_io_factory.hpp"

#include "../layers/ogr_vector_layer.hpp"

using namespace boost;
using namespace std;

#include <iostream>

shared_ptr<layer> gilviewer_file_io_shp::load(const string &filename, const ptrdiff_t top_left_x, const ptrdiff_t top_left_y, const ptrdiff_t dim_x, const ptrdiff_t dim_y)
{
    return shared_ptr<layer>(new ogr_vector_layer(filesystem::basename(filename),filename));
}

void gilviewer_file_io_shp::save(shared_ptr<layer> layer, const string &filename)
{
    shared_ptr<ogr_vector_layer> ogr_layer = dynamic_pointer_cast<ogr_vector_layer>(layer);
    if(!ogr_layer)
        throw invalid_argument("Bad layer type!\n");

    OGRSFDriver *poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName( "ESRI Shapefile" );
    if( poDriver == NULL )
        throw invalid_argument("ESRI Shapefile driver is not available. Did you forget to call OGRRegisterAll()?\n");

    OGRDataSource *poDS = poDriver->CreateDataSource( filename.c_str(), NULL );
    if( poDS == NULL )
    {
        ostringstream oss;
        oss << "Creation of output file " << filename << " failed.\n";
        throw ios_base::failure(oss.str());
    }

    OGRLayer *poLayer = poDS->CreateLayer( "", NULL, wkbUnknown, NULL );
    if( poLayer == NULL )
        throw ios_base::failure("Creation of layer failed.\n");

    std::vector<std::pair<geometry_types,OGRFeature*> > geometries = ogr_layer->geometries_features();
    typedef std::vector<std::pair<geometry_types,OGRFeature*> >::const_iterator const_iterator;
    const_iterator itb=geometries.begin(), ite=geometries.end();
    for(;itb!=ite;++itb)
    {
        OGRFeature *poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );
        // --------- Point --------- //
        {
            try
            {
                OGRPoint *poPoint = boost::get<OGRPoint*>(itb->first);
                poFeature->SetGeometry(poPoint);
                if( poLayer->CreateFeature( poFeature ) != OGRERR_NONE )
                    throw invalid_argument("Failed to create feature in shapefile.\n");
                OGRFeature::DestroyFeature( poFeature );
                continue;
            }
            catch(const bad_get& e)
            {
            }
        }
        // --------- MultiPoint --------- //
        {
            try
            {
                OGRMultiPoint *poMPoint = boost::get<OGRMultiPoint*>(itb->first);
                poFeature->SetGeometry(poMPoint);
                if( poLayer->CreateFeature( poFeature ) != OGRERR_NONE )
                    throw invalid_argument("Failed to create feature in shapefile.\n");
                OGRFeature::DestroyFeature( poFeature );
                continue;
            }
            catch(const bad_get& e)
            {
            }
        }
        // --------- Polygon --------- //
        {
            try
            {
                OGRPolygon *poPolygon = boost::get<OGRPolygon*>(itb->first);
                poFeature->SetGeometry(poPolygon);
                if( poLayer->CreateFeature( poFeature ) != OGRERR_NONE )
                    throw invalid_argument("Failed to create feature in shapefile.\n");
                OGRFeature::DestroyFeature( poFeature );
                continue;
            }
            catch(const bad_get& e)
            {
            }
        }
        // --------- MultiPolygon --------- //
        {
            try
            {
                OGRMultiPolygon *poMPolygon = boost::get<OGRMultiPolygon*>(itb->first);
                poFeature->SetGeometry(poMPolygon);
                if( poLayer->CreateFeature( poFeature ) != OGRERR_NONE )
                    throw invalid_argument("Failed to create feature in shapefile.\n");
                OGRFeature::DestroyFeature( poFeature );
                continue;
            }
            catch(const bad_get& e)
            {
            }
        }
        // --------- LinearRing --------- //
        {
            try
            {
                OGRLinearRing *poLinear = boost::get<OGRLinearRing*>(itb->first);
                poFeature->SetGeometry(poLinear);
                if( poLayer->CreateFeature( poFeature ) != OGRERR_NONE )
                    throw invalid_argument("Failed to create feature in shapefile.\n");
                OGRFeature::DestroyFeature( poFeature );
                continue;
            }
            catch(const bad_get& e)
            {
            }
        }
        // --------- LineString --------- //
        {
            try
            {
                OGRLineString *poLinear = boost::get<OGRLineString*>(itb->first);
                poFeature->SetGeometry(poLinear);
                if( poLayer->CreateFeature( poFeature ) != OGRERR_NONE )
                    throw invalid_argument("Failed to create feature in shapefile.\n");
                OGRFeature::DestroyFeature( poFeature );
                continue;
            }
            catch(const bad_get& e)
            {
            }
        }
        // --------- MultiLineString --------- //
        {
            try
            {
                OGRMultiLineString *poMLinear = boost::get<OGRMultiLineString*>(itb->first);
                poFeature->SetGeometry(poMLinear);
                if( poLayer->CreateFeature( poFeature ) != OGRERR_NONE )
                    throw invalid_argument("Failed to create feature in shapefile.\n");
                OGRFeature::DestroyFeature( poFeature );
                continue;
            }
            catch(const bad_get& e)
            {
            }
        }
    }
    OGRDataSource::DestroyDataSource( poDS );
}

boost::shared_ptr<gilviewer_file_io_shp> create_gilviewer_file_io_shp()
{
    return shared_ptr<gilviewer_file_io_shp>(new gilviewer_file_io_shp());
}

bool gilviewer_file_io_shp::Register()
{
    gilviewer_io_factory::instance()->Register("shp", create_gilviewer_file_io_shp);
    pair<string,string> familly_description = make_pair<string,string>("Vector files","SHP files");
    pair< string, pair<string,string> > to_insert = make_pair< string, pair<string,string> >( "shp", familly_description );
    gilviewer_io_factory::instance()->metadata().insert( to_insert );
    return true;
}

bool register_shp_ok = gilviewer_file_io_shp::Register();

#endif // GILVIEWER_USE_GDAL
