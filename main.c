/* Retro - Retro style programming
 * Copyright (C) 2016 Imran Haider
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Retro is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Retro.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <retro.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    /* Initialize the Retro system
     */
    retro_init(argc, argv);

    /* Set the framebuffer size to 128000 bytes (or 128 KB)
     */
    retro_video_set_framebuffer_size(128000);

    /* Set the frame width to 640 pixels
     */
    retro_video_set_framebuffer_width(640);

    /* Set the frame rate of the video
     */
    retro_video_set_fps(60);

    /* Set the viewport position to (0,0)
     */
    retro_video_set_viewport_position(0, 0);

    /* Set the viewport size to 320x200 pixels
     */
    retro_video_set_viewport_size(320, 200);

    /* Get a pointer to the video memory
     */
    unsigned char *screen = retro_video_get_framebuffer();

    /* Update the video memory content
     */
    int i, x, y, offset;

    for (i=0; i<100000; i+=10) {
        for (y=0; y<200; ++y) {
            for (x=0; x<320; ++x) {
                offset = y * 640 + x;

                /* red */
                screen[offset * 3 + 0] = i % 255;

                /* green */
                screen[offset * 3 + 1] = i % 255;

                /* blue */
                screen[offset * 3 + 2] = 0;
            }
        }

        /* Wait for vertical retrace
         */
        while (retro_video_get_vsync_signal());
        while (!retro_video_get_vsync_signal());
    }

    sleep(20);
}

