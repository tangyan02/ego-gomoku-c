#pragma once

Color ** getEmptyMap();

Color** readMap();

Color** readMap(char* path);

Color** readMapFromOpennings(int lineNum, Color** map);

Color nextColorForOpennings(Color** map);


