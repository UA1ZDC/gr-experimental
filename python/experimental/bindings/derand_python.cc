/*
 * Copyright 2022 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/***********************************************************************************/
/* This file is automatically generated using bindtool and can be manually edited  */
/* The following lines can be configured to regenerate this file during cmake      */
/* If manual edits are made, the following tags should be modified accordingly.    */
/* BINDTOOL_GEN_AUTOMATIC(0)                                                       */
/* BINDTOOL_USE_PYGCCXML(0)                                                        */
/* BINDTOOL_HEADER_FILE(derand.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(39407cfd55ba7fd120636ae40489c631)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/experimental/derand.h>
// pydoc.h is automatically generated in the build directory
#include <derand_pydoc.h>

void bind_derand(py::module& m)
{

    using derand    = gr::experimental::derand;


    py::class_<derand, gr::block, gr::basic_block,
        std::shared_ptr<derand>>(m, "derand", D(derand))

        .def(py::init(&derand::make),
           D(derand,make)
        )
        



        ;




}








