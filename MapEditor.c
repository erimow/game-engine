#include "MapEditor.h"
#include "constants.h"


bool ME_LoadMap(char* filename){
    // ts->tilePixelSideLength = TILE_SIZE;
    // ts->filename = filename;
   FILE *file = fopen(filename, "r");
  if (file == NULL) {
    perror("Failed to open file");
    return false;
  }

  // Determine file size
  fseek(file, 0, SEEK_END);
  long fileSize = ftell(file);
  fseek(file, 0, SEEK_SET); // Reset file pointer to the beginning

  // Allocate memory for the file content
  char *content =
      (char *)malloc((fileSize + 1) * sizeof(char)); // +1 for null terminator
  if (content == NULL) {
    perror("Failed to allocate memory");
    fclose(file);
    return false;
  }

  // Read file content
  int rows = 0;
  size_t totalBytesRead = 0;
  char line[1024];
  while (fgets(line, sizeof(line), file)) {
    size_t len = strlen(line);
    // if (line[len - 1] == '\n')
    // line[len - 1] = '\0'; // Remove newline character
    strcpy(content + totalBytesRead, line);
    rows++;
    totalBytesRead += len - 1; // remove newline character or null terminator
  }
  content[totalBytesRead] = '\0'; // Null-terminate the final string
  // printf("content=%s\n", content);

  // ts->imageheight = rows*ts->tilepixelsidelength;
  // ts->imagewidth = (int)(totalbytesread / rows)*ts->tilepixelsidelength;
  // tm->mapHeight = rows;
  // tm->mapWidth = (int)(totalBytesRead / rows);
  // printf("mapHeight: %d\nmapWidth: %d\nTotal bytes: %zu\n", ts->imageHeight/TILE_SIZE,
  //        ts->imageWidth/TILE_SIZE, totalBytesRead);

  fclose(file);
  return true;
}
