#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#ifndef WINDOW_SIZE
#define WINDOW_SIZE
#define WIDTH 1282
#define HEIGHT 732
#endif // WINDOW_SIZE

void Load_Texture( char *filename, GLuint *textureID, int swarp, int twarp, int filter);
void Show_Background(GLuint texture);
void nextLine(int k);
void GlSettingsOn(GLuint texture);
void GlSettingsOff();

#endif // TEXTURE_HPP
