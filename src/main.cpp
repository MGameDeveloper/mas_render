#include "mas/mem_tracker.h"
#include "mas/log.h"
#include "mas/camera.h"
#include "mas/shader.h"
#include "mas/render.h"
#include "mas/window.h"
#include "mas/clut.h"
#include "mas/texture.h"
#include "mas/input.h"
#include "mas/cmds.h"
#include "mas/keys.h"

int main(int argc, const char* argv[])
{
	mas_window  wnd              ("BC_RENDERER", glm::vec2(913, 800));
	mas_shader  simple_shader    ("assets/shaders/simple.vrtx", "assets/shaders/simple.frag");
	mas_shader  battlecity_shader("assets/shaders/battlecity.vrtx", "assets/shaders/battlecity.frag");
	mas_camera  camera           (256, 224, -10, 1000);
	mas_render  canvas           (10000);
	mas_clut    clut             (8, 4);
	
	mas_texture::init();
	mas_keys::init();
	
	mas_texture brick_texture ("assets/textures/bc_brick_tile.png");
	mas_texture nes_font      ("assets/fonts/nes.png");
	mas_texture namcot_font   ("assets/fonts/namco.png");
	mas_texture nes_brick_font(&nes_font, &brick_texture, 32);

	while (!wnd.is_closing())
	{
		wnd.poll_events();

		canvas.clear();

		canvas.begin(&camera, &simple_shader);

		glm::vec2 pos(0.f);
		glm::vec2 size(8.f);
		glm::vec2 count(wnd.get_size().x / size.x, wnd.get_size().y / size.y);

		for (i32 y = 0; y < count.y; ++y)
		{
			if (canvas.is_full())
				break;

			for (i32 x = 0; x < count.x; ++x)
			{
				//canvas.draw_rect(pos, size, mas_color::rand_color());
				pos.x += size.x;

				if (canvas.is_full())
					break;
			}

			pos.x = 0;
			pos.y += size.y;
		}

		canvas.end();

		wnd.swap_buffers();
	}
	return 0;
}