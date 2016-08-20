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

#ifndef RETRO_INCLUDED
#define RETRO_INCLUDED

/* Initialize the Retro system.
 */
int retro_init(int argc, char **argv);

/* Destroy the Retro system.
 */
void retro_destroy();

/* Returns a pointer to the video memory. The size of the buffer is
 * set by the function retro_video_set_framebuffer_size.
 */
void *retro_video_get_framebuffer();

/* Sets the video memory size, in pixels.
 */
void retro_video_set_framebuffer_size(int npixels);

/* Shapes the video memory in terms of the width. The height is computed based
 * on the value specified in retro_video_set_framebuffer_size.
 */
void retro_video_set_framebuffer_width(int npixels);

/* Position the visible viewport in the framebuffer.
 */
void retro_video_set_viewport_position(int x, int y);

/* Set the size of the visible viewport. This is effectively setting the
 * screen resolution.
 */
void retro_video_set_viewport_size(int width, int height);

/* Returns 0 if the screen is in the middle of a paint, 1 if the screen
 * is in resting state between frames.
 */
int retro_video_get_vsync_signal();

/* Set the video frame rate.
 */
void retro_video_set_fps(int fps);

#endif

