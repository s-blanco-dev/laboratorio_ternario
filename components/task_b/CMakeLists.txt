idf_component_register(SRCS "task_b.c"
                       INCLUDE_DIRS "include"
                       PRIV_INCLUDE_DIRS ""
                       PRIV_REQUIRES "driver"
                       REQUIRES
                        led_strip
                        rgb_led
                       )
