/***********************************************************************

This file is part of the GilViewer project source files.

GilViewer is an open source 2D viewer (raster and vector) based on Boost
GIL and wxWidgets.


Homepage: 

	http://launchpad.net/gilviewer
	
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

struct transparency_functor
{
	transparency_functor(const double min_alpha, const double max_alpha):
		m_min_alpha(min_alpha),
		m_max_alpha(max_alpha)
		{}

	template<class PixelT>
	bool operator()(const PixelT& src) const
	{
		if(m_min_alpha<m_max_alpha)
			return m_min_alpha<=at_c<0>(src) && at_c<0>(src)<=m_max_alpha;
		else
			return m_min_alpha<=at_c<0>(src) || at_c<0>(src)<=m_max_alpha;
	}

	const double m_min_alpha, m_max_alpha;
};


template<>
bool transparency_functor::operator()<rgb8_pixel_t>(const rgb8_pixel_t& src) const
{
	if(m_min_alpha<m_max_alpha)
		return m_min_alpha<=at_c<0>(src) && at_c<0>(src)<=m_max_alpha
			&& m_min_alpha<=at_c<1>(src) && at_c<1>(src)<=m_max_alpha
			&& m_min_alpha<=at_c<2>(src) && at_c<2>(src)<=m_max_alpha;
	else
		return (m_min_alpha<=at_c<0>(src) || at_c<0>(src)<=m_max_alpha)
			&& (m_min_alpha<=at_c<1>(src) || at_c<1>(src)<=m_max_alpha)
			&& (m_min_alpha<=at_c<2>(src) || at_c<2>(src)<=m_max_alpha);
}

template<>
bool transparency_functor::operator()<rgb16_pixel_t>(const rgb16_pixel_t& src) const
{
	if(m_min_alpha<m_max_alpha)
		return m_min_alpha<=at_c<0>(src) && at_c<0>(src)<=m_max_alpha
			&& m_min_alpha<=at_c<1>(src) && at_c<1>(src)<=m_max_alpha
			&& m_min_alpha<=at_c<2>(src) && at_c<2>(src)<=m_max_alpha;
	else
		return (m_min_alpha<=at_c<0>(src) || at_c<0>(src)<=m_max_alpha)
			&& (m_min_alpha<=at_c<1>(src) || at_c<1>(src)<=m_max_alpha)
			&& (m_min_alpha<=at_c<2>(src) || at_c<2>(src)<=m_max_alpha);
}

struct apply_transparency_functor
{
	apply_transparency_functor(const double min_alpha, const double max_alpha, const unsigned char alpha):
		m_alpha(alpha),
		m_zero(0),
		m_transparencyFonctor(min_alpha, max_alpha)
		{}

	template<class PixelTSrc>
	gray8_pixel_t operator()(const PixelTSrc& src) const
	{
		if(m_transparencyFonctor(src))
			return m_zero;
		return m_alpha;
	}

	const gray8_pixel_t m_alpha, m_zero;
	transparency_functor m_transparencyFonctor;
};
