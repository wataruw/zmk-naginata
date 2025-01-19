target_sources_ifdef(CONFIG_NAGINATA app PRIVATE src/behaviors/behavior_naginata.c)
target_sources_ifdef(CONFIG_NAGINATA app PRIVATE src/naginata_func.c)
target_sources_ifdef(CONFIG_NAGINATA app PRIVATE src/nglist.c)
target_sources_ifdef(CONFIG_NAGINATA app PRIVATE src/nglistarray.c)

zephyr_include_directories(include)
