idf_component_register(
    SRCS tusb_composite_main.c "Mouse.c" "Keyboard.c" "TimeBased.c" "Macro.c" "Storage.c"
                    INCLUDE_DIRS "." "../includes")
