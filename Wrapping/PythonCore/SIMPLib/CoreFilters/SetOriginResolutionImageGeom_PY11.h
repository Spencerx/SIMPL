#pragma once

/**
 * @brief This header file is genererated from a program. Make changes with caution.
 *
 *
 * This header file wraps the SIMPL class SetOriginResolutionImageGeom located at
 * SIMPLib/CoreFilters/SetOriginResolutionImageGeom.h.
 * The Python bindings are created using Pybind11.
 * @date 2018:04:04 12::59::23.115
 */

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "SIMPLib/CoreFilters/SetOriginResolutionImageGeom.h"

/**
 * @brief This defines a C++11 alias so loading the file by itself into an IDE
 * will allow the proper code completion for a wrapped std::shared_ptr<> class.
 */
#ifndef PySharedPtrClass_TEMPLATE
#define PySharedPtrClass_TEMPLATE
template <typename T> using PySharedPtrClass = py::class_<T, std::shared_ptr<T>>;
#endif

/**
 * @brief This allows filters declared in a plugin to be wrapped
 */
#ifndef PyAbstractFilterSharedPtrClass_TEMPLATE
#define PyAbstractFilterSharedPtrClass_TEMPLATE
template <typename T> using PyAbstractFilterSharedPtrClass = py::class_<T, AbstractFilter, std::shared_ptr<T>>;
#endif

/**
 * @brief
 * @param m The Python module
 * @param parent The parent class of the wrapped class
 * @return A std::shared_ptr<T> wrapped instance of the wrapped class properly
 * initialized.
 */
PySharedPtrClass<SetOriginResolutionImageGeom> pybind11_init_SIMPLib_SetOriginResolutionImageGeom(py::module& m
#ifdef simpl_EXPORTS
                                                                                                  ,
                                                                                                  PySharedPtrClass<AbstractFilter>& parent
#endif
)
{
#ifdef simpl_EXPORTS
  PySharedPtrClass<SetOriginResolutionImageGeom> instance(m, "SetOriginResolutionImageGeom", parent);
#else
  py::module::import("simpl");
  PyAbstractFilterSharedPtrClass<SetOriginResolutionImageGeom> instance(m, "SetOriginResolutionImageGeom");
#endif

  instance
      /* Property accessors for DataContainerName */
      .def_property("DataContainerName", &SetOriginResolutionImageGeom::getDataContainerName, &SetOriginResolutionImageGeom::setDataContainerName)
      /* Property accessors for ChangeOrigin */
      .def_property("ChangeOrigin", &SetOriginResolutionImageGeom::getChangeOrigin, &SetOriginResolutionImageGeom::setChangeOrigin)
      /* Property accessors for Origin */
      .def_property("Origin", &SetOriginResolutionImageGeom::getOrigin, &SetOriginResolutionImageGeom::setOrigin)
      /* Property accessors for ChangeResolution */
      .def_property("ChangeResolution", &SetOriginResolutionImageGeom::getChangeResolution, &SetOriginResolutionImageGeom::setChangeResolution)
      /* Property accessors for Resolution */
      .def_property("Resolution", &SetOriginResolutionImageGeom::getResolution, &SetOriginResolutionImageGeom::setResolution);

  /* Return the instance */
  return instance;
}