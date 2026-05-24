idf_component_register(SRCS "task_c.c"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES driver
                       REQUIRES rgb_led)
