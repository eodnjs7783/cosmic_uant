project(CFE_UANT_APP C)

set(APP_SRC_FILES
  fsw/src/uant_app.c
  fsw/src/uant_app_cmds.c
  fsw/src/uant_app_utils.c
)

if (CFE_EDS_ENABLED_BUILD)
  list(APPEND APP_SRC_FILES
    fsw/src/uant_app_eds_dispatch.c
  )
else()
  list(APPEND APP_SRC_FILES
    fsw/src/uant_app_dispatch.c
  )
endif()

# Create the app module
add_cfe_app(uant_app ${APP_SRC_FILES})

#target_include_directories(uant_app PUBLIC fsw/inc)
target_include_directories(uant_app PUBLIC
    fsw/inc                       # 앱 내부 헤더
    ${CMAKE_CURRENT_SOURCE_DIR}/device/inc/uant   # uant.h가 있는 경로
)


target_include_directories(uant_app PUBLIC ../../cfe/modules/srl/config)
target_include_directories(uant_app PUBLIC /usr/include)
# Include the public API from uant_lib to demonstrate how
# to call library-provided functions
#add_cfe_app_dependency(uant_app)

# Add table
add_cfe_tables(uant_app fsw/tables/uant_app_tbl.c)

# If UT is enabled, then add the tests from the subdirectory
# Note that this is an app, and therefore does not provide
# stub functions, as other entities would not typically make
# direct function calls into this application.
if (ENABLE_UNIT_TESTS)
  add_subdirectory(unit-test)
endif (ENABLE_UNIT_TESTS)



