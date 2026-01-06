
set(GENERATED_ASSETS "")
set(OUT_DIR "${CMAKE_BINARY_DIR}/generated_assets")
file(MAKE_DIRECTORY "${OUT_DIR}")

# Find all image assets
file(GLOB SVG_ASSETS
  "${CMAKE_CURRENT_SOURCE_DIR}/icons/*.svg"
)

set(ICON_SIZES 16 24 32 48)

# converters
find_program(INKSCAPE inkscape)
find_program(RSVG_CONVERT rsvg-convert)
if(NOT INKSCAPE AND NOT RSVG_CONVERT)
    message(FATAL_ERROR "Install inkscape or rsvg-convert to render SVG icons.")
endif()

set(GENERATED_PNGS "")

# Convert each image using the ESP-IDF LVGL port function
foreach(ASSET ${SVG_ASSETS})
  get_filename_component(icon_name "${ASSET}" NAME_WE)

  foreach(sz ${ICON_SIZES})

    set(png_out "${OUT_DIR}/${icon_name}_${sz}.png")

    if(INKSCAPE)
        execute_process(
            COMMAND "${INKSCAPE}" "${ASSET}"
                    --export-type=png
                    --export-filename="${png_out}"
                    -w ${sz} -h ${sz}
            RESULT_VARIABLE r
            OUTPUT_QUIET
            ERROR_VARIABLE e
        )
    else()
        execute_process(
            COMMAND "${RSVG_CONVERT}" -w ${sz} -h ${sz}
                    -o "${png_out}" "${ASSET}"
            RESULT_VARIABLE r
            OUTPUT_QUIET
            ERROR_VARIABLE e
        )
    endif()

    if(NOT r EQUAL 0 OR NOT EXISTS "${png_out}")
        message(FATAL_ERROR "Failed to render ${ASSET} -> ${png_out}\n${e}")
    endif()
    
    # Use the ESP-IDF LVGL port's function for image conversion
    lvgl_port_create_c_image("${png_out}" "${OUT_DIR}" "ARGB8565" "NONE")
    
  endforeach()
endforeach()

idf_component_register(
  INCLUDE_DIRS "${OUT_DIR}"
  REQUIRES lvgl
)
lvgl_port_add_images(${COMPONENT_LIB} "${OUT_DIR}")
