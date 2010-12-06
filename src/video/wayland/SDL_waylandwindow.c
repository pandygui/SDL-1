#include "SDL_config.h"

#include "../SDL_sysvideo.h"
#include "SDL_waylandwindow.h"
#include "SDL_waylandvideo.h"



int Wayland_CreateWindow(_THIS, SDL_Window * window)
{
    printf("window create\n");
    SDL_WaylandWindow *data;
	data = malloc(sizeof *data);
	if (data == NULL)
		return 0;
    SDL_WaylandData *c;
    c = _this->driverdata;
    window->driverdata = data;
	struct wl_visual *visual;
	int i;
	EGLint name, stride, attribs[] = {
		EGL_WIDTH,	0,
		EGL_HEIGHT,	0,
		EGL_DRM_BUFFER_FORMAT_MESA, EGL_DRM_BUFFER_FORMAT_ARGB32_MESA,
		EGL_DRM_BUFFER_USE_MESA,    EGL_DRM_BUFFER_USE_SCANOUT_MESA,
		EGL_NONE
	};
    

	data->surface =
		wl_compositor_create_surface(c->compositor);
	wl_surface_set_user_data(data->surface, data);

	glGenRenderbuffers(2, data->rbo);
	visual = wl_display_get_premultiplied_argb_visual(c->display);
	for (i = 0; i < 2; i++) {
		glBindRenderbuffer(GL_RENDERBUFFER, data->rbo[i]);

		attribs[1] = window->w;
		attribs[3] = window->h;
		data->image[i] =
			eglCreateDRMImageMESA(c->display, attribs);
		glEGLImageTargetRenderbufferStorageOES(GL_RENDERBUFFER,
						       data->image[i]);
		eglExportDRMImageMESA(c->display, data->image[i],
				      &name, NULL, &stride);
		data->buffer[i] =
			wl_drm_create_buffer(c->drm, name,
					     window->w, window->h,
					     stride, visual);
	}

	data->current = 0;
    glEGLImageTargetRenderbufferStorageOES(GL_RENDERBUFFER, data->image[0]);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,
				  GL_COLOR_ATTACHMENT0,
				  GL_RENDERBUFFER,
				  data->rbo[data->current]);

	wl_surface_attach(data->surface,
			  data->buffer[data->current]);
	wl_surface_map(data->surface,
		       0, 0,
		       window->w, window->h);
    printf("window %d %d\n", window->w, window->h);

	glClearColor(0, 0, 0, 0.5);

    return 0;
}