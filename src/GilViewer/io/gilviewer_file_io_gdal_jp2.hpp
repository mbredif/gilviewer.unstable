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
#ifndef GILVIEWER_FILE_IO_GDAL_JPG2_HPP
#define GILVIEWER_FILE_IO_GDAL_JPG2_HPP

#include "../config/config.hpp"
#if GILVIEWER_USE_GDALJP2

#include "gilviewer_file_io_image.hpp"

class gilviewer_file_io_gdal_jp2 : public gilviewer_file_io_image
{
public:
    virtual ~gilviewer_file_io_gdal_jp2() {}

    virtual boost::shared_ptr<layer> load(const std::string &filename, const std::ptrdiff_t top_left_x=0, const std::ptrdiff_t top_left_y=0, const std::ptrdiff_t dim_x=0, const std::ptrdiff_t dim_y=0);
    virtual void save(boost::shared_ptr<layer> layer, const std::string &filename);

    virtual std::string build_and_get_infos(const std::string &filename);

    static bool Register();
    friend boost::shared_ptr<gilviewer_file_io_gdal_jp2> create_gilviewer_file_io_gdal_jp2();

private:
    gilviewer_file_io_gdal_jp2() {}
};

#endif // GILVIEWER_USE_GDALJP2
#endif // GILVIEWER_FILE_IO_GDAL_JPG2_HPP
