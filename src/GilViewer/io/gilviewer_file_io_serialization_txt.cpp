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
#include "gilviewer_file_io_serialization_txt.hpp"

#include <boost/filesystem/convenience.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <fstream>

#include "gilviewer_io_factory.hpp"

#include "../layers/simple_vector_layer.hpp"

using namespace boost;
using namespace std;

shared_ptr<layer> gilviewer_file_io_serialization_txt::load(const string &filename)
{
    simple_vector_layer simple_layer;
    {
        // create and open an archive for input
        ifstream ifs(filename.c_str());
        boost::archive::text_iarchive ia(ifs);
        // read class state from archive
        ia >> BOOST_SERIALIZATION_NVP(simple_layer);
        // archive and stream closed when destructors are called
    }
    return shared_ptr<layer>(new simple_vector_layer(simple_layer));
}

void gilviewer_file_io_serialization_txt::save(shared_ptr<layer> layer, const string &filename)
{
    shared_ptr<simple_vector_layer> simple_layer2 = dynamic_pointer_cast<simple_vector_layer>(layer);
    if(!simple_layer2)
        throw invalid_argument("Bad layer type!\n");

    ofstream ofs(filename.c_str());
    // save data to archive
    {
        boost::archive::text_oarchive oa(ofs);
        // write class instance to archive
        simple_vector_layer simple_layer = *simple_layer2;
        oa << BOOST_SERIALIZATION_NVP(simple_layer);
        // archive and stream closed when destructors are called
    }
}

boost::shared_ptr<gilviewer_file_io_serialization_txt> create_gilviewer_file_io_serialization_txt()
{
    return shared_ptr<gilviewer_file_io_serialization_txt>(new gilviewer_file_io_serialization_txt());
}

bool gilviewer_file_io_serialization_txt::Register()
{
    gilviewer_io_factory::instance()->Register("txt", create_gilviewer_file_io_serialization_txt);
    return true;
}

bool register_serialization_txt_ok = gilviewer_file_io_serialization_txt::Register();
