function(OSInformation)

  if(APPLE)
    EXEC_PROGRAM(uname ARGS -v  OUTPUT_VARIABLE DARWIN_VERSION)
    STRING(REGEX MATCH "[0-9]+" DARWIN_VERSION ${DARWIN_VERSION})
    #message(STATUS "DARWIN_VERSION: ${DARWIN_VERSION}")
    if (DARWIN_VERSION EQUAL 14) # macOS 10.10 Yosemite
      # message(STATUS "Found macOS 10.10 Yosemite as the host. Darwin Version:${DARWIN_VERSION}")
      set(CMAKE_MACOS_NAME "Yosemite")
      set(CMAKE_MACOS_VERSION "10.10")
    endif ()

    if (DARWIN_VERSION EQUAL 15) # macOS 10.11 El Capitan
      # message(STATUS "Found macOS 10.11 El Capitan as the host. Darwin Version:${DARWIN_VERSION}")
      set(CMAKE_MACOS_NAME "El Capitan")
      set(CMAKE_MACOS_VERSION "10.11")
    endif ()

    if (DARWIN_VERSION EQUAL 16) # macOS 10.12 Sierra (Xcode 8.x or Xcode 9.x)
      # message(STATUS "Found macOS 10.12 Sierra as the host. Darwin Version:${DARWIN_VERSION}")
      set(CMAKE_MACOS_NAME "Sierra")
      set(CMAKE_MACOS_VERSION "10.12")
    endif ()
    
    if (DARWIN_VERSION EQUAL 17) # macOS 10.13 High Sierra (Xcode 9.x)
      # message(STATUS "Found macOS 10.13 High Sierra as the host. Darwin Version:${DARWIN_VERSION}")  
      set(CMAKE_MACOS_NAME "High Sierra")
      set(CMAKE_MACOS_VERSION "10.13")
    endif ()

    if (DARWIN_VERSION EQUAL 18) # macOS 10.14 Mojave (Xcode 10.x)
      # message(STATUS "Found macOS 10.14 Mojave as the host. Darwin Version:${DARWIN_VERSION}")
      set(CMAKE_MACOS_NAME "Mojave")
      set(CMAKE_MACOS_VERSION "10.14")
    endif ()

    if (DARWIN_VERSION EQUAL 19) # macOS 10.15 Catalina (Xcode 11.x or Xcode 12.x)
      # message(STATUS "Found macOS 10.15 Catalina as the host. Darwin Version:${DARWIN_VERSION}")
      set(CMAKE_MACOS_NAME "Catalina")
      set(CMAKE_MACOS_VERSION "10.15")
    endif ()

    if (DARWIN_VERSION EQUAL 20) # macOS 11.00 Big Sur (Xcode 12.x)
      # message(STATUS "Found macOS 11.00 Big Sur as the host. Darwin Version:${DARWIN_VERSION}")
      set(CMAKE_MACOS_NAME "Big Sur")
      set(CMAKE_MACOS_VERSION "11.00")
    endif ()

    message(STATUS "* System: MacOS ${CMAKE_MACOS_NAME}")
    message(STATUS "* Version: ${CMAKE_MACOS_VERSION}")

  else()

    message(STATUS "* System: ${CMAKE_SYSTEM_NAME}")
    message(STATUS "* Version: ${CMAKE_SYSTEM_VERSION}")

  endif()
  message(STATUS "* Processor: ${CMAKE_SYSTEM_PROCESSOR}")

endfunction()



#------------------------------------------------------------------------------------
# Print out a Summary Section:
message(STATUS "*******************************************************************************")
message(STATUS "* DREAM.3D Configuration Summary")
message(STATUS "* BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
OSInformation()
message(STATUS "* DREAM3D_DATA_DIR: ${DREAM3D_DATA_DIR}")
message(STATUS "* SIMPL_ENABLE_PYTHON: ${SIMPL_ENABLE_PYTHON}")
if(SIMPL_ENABLE_PYTHON)
  message(STATUS "* PYTHON_EXECUTABLE: ${PYTHON_EXECUTABLE}")
  message(STATUS "* SIMPL_USE_ANACONDA_PYTHON: ${SIMPL_USE_ANACONDA_PYTHON}")
  message(STATUS "* SIMPL_CONDA_EXECUTABLE: ${SIMPL_CONDA_EXECUTABLE}")
  message(STATUS "* ANACONDA_DIR: ${ANACONDA_DIR}")
  message(STATUS "* ANACONDA_ENVIRONMENT_NAME: ${ANACONDA_ENVIRONMENT_NAME}")
  message(STATUS "* SIMPL_ENABLE_ANACONDA_PACKAGE: ${SIMPL_ENABLE_ANACONDA_PACKAGE}")
  message(STATUS "* SIMPL_ENABLE_ANACONDA_LOCAL_INSTALL: ${SIMPL_ENABLE_ANACONDA_LOCAL_INSTALL}")
endif()
message(STATUS "* -------------- Dependent Libraries -------------------------------------------")
message(STATUS "* DREAM3_SDK: ${DREAM3D_SDK}")
message(STATUS "* Eigen (${Eigen3_VERSION}) ${Eigen3_DIR}")
message(STATUS "* Haru (${libharu_VERSION}) ${libharu_DIR}")
message(STATUS "* HDF5 (${HDF5_VERSION}) ${HDF5_INSTALL}")
message(STATUS "* ITK (${ITK_VERSION}) ${ITK_DIR}")
message(STATUS "* Pybind11 (${pybind11_VERSION}) ${pybind11_DIR}")
message(STATUS "* Qt5 (${Qt5_VERSION}) ${Qt5_DIR}")
message(STATUS "* Qwt (${QWT_VERSION}) ${QWT_INSTALL}")
message(STATUS "* TBB (${TBB_VERSION}) ${TBB_INSTALL_DIR}")
message(STATUS "* -------------- Core Libraries -----------------------------------------------")
message(STATUS "*   BrandedSIMPLView_DIR: ${BrandedSIMPLView_DIR}")
set(ALL_PROJECTS CMP SIMPLProj SIMPLViewProj DREAM3DProj)
foreach(proj ${ALL_PROJECTS})
  cmpGitRevisionString(PROJECT_NAME ${proj})
  get_property(githash GLOBAL PROPERTY ${proj}_GIT_HASH)
  message(STATUS "*   ${proj}: ${${proj}_SOURCE_DIR}  Git Hash: ${githash}")
endforeach(proj )
message(STATUS "* -------------- Plugins ------------------------------------------------------")
foreach(d3dPlugin ${DREAM3D_ALL_PLUGINS})
  get_property(PluginNumFilters GLOBAL PROPERTY ${d3dPlugin}_NumFilters)
  get_property(PluginGitHash GLOBAL PROPERTY ${d3dPlugin}_GIT_HASH)
  get_property(PluginCommitDate GLOBAL PROPERTY ${d3dPlugin}_GIT_COMMIT_DATE)
  message(STATUS "*   ${d3dPlugin}: [${DREAM3D_ENABLE_${d3dPlugin}}] ${PluginNumFilters} Filters  Git:${PluginGitHash} ${PluginCommitDate}")
endforeach()
message(STATUS "*******************************************************************************")




