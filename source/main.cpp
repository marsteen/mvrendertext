
#include <iostream>
#include <cstdint>
#include <cstring>
#include <mvpng/inc/mv_graphics_png.h>


using namespace std;

#define SIZE_array_Px437_ATI_8x14 8224
extern uint8_t array_Px437_ATI_8x14[];

#define TABLEAU_SIZE 256


static uint8_t* init_text()
{
    uint8_t* data = new uint8_t[TABLEAU_SIZE * TABLEAU_SIZE + 128];
    memset(data, 0, TABLEAU_SIZE * TABLEAU_SIZE + 128);
    return data;
}


void write_png(const uint8_t* data)
{
    mv_graphics_png_write("out.png", (const char*) data, TABLEAU_SIZE, TABLEAU_SIZE, 8);
}




static void renderText(const char* text, uint8_t* data, int x, int y)
{

    int imageoffset = y * 256 * 16 + x * 8;
    for (int i = 0; text[i] != 0; i++)
    {
        int textoffset = 16 + text[i] * 16;
        for (int ln = 0; ln < 14; ln++)
        {
            uint16_t tbyte = ((uint16_t*) array_Px437_ATI_8x14)[textoffset + ln];
            for (int bit = 0; bit < 16; bit++)
            {
                if (tbyte & (1 << bit))
                {
                    data[imageoffset + ln*256 + bit] = 0xFF;
                }
            }
        }
        imageoffset += 8;
        x += 1;
        if (x >= 32)
        {
            x = 0;
            y += 1;
            if (y > 30)
            {
                break;
            }
            imageoffset += 256 * 16;
        }
    }

}


static void renderTextCenter(const char* text, uint8_t* data, int y)
{
    int x = 32 - strlen(text) / 2;
    renderText(text, data, x, y);
}

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        cout << "HALLO" << endl;
        uint8_t* data = init_text();
        renderTextCenter(argv[1], data, 8);
        /*
        for (int x = 0; x < 32; x++)
        for (int y = 0; y < 16; y++)
        {
            renderText("X", data, x, y);
        }
        */
        cout << "write png.." << endl;
        write_png(data);
        cout << "ok" << endl;

    }
    else
    {
        cout << "usage: mvrendertext <text>" << endl;
    }

    return 0;
}