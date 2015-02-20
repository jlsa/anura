#ifndef CAIRO_HPP_INCLUDED
#define CAIRO_HPP_INCLUDED

#include <string>
#include <cairo.h>

#include "formula_callable.hpp"
#include "graphics.hpp"
#include "surface.hpp"
#include "texture.hpp"

namespace graphics
{

// Basic glue class between cairo and Anura's texture/surface formats.
// Use a cairo_context when you want to use vector graphics to render
// a texture. Create the cairo_context, use get() to get out the cairo_t*
// and then perform whatever cairo draw calls you want. Then call write()
// to extract a graphics::texture for use in Anura.
class cairo_context
{
public:
	cairo_context(int w, int h);
	~cairo_context();

	cairo_t* get() const;
	surface get_surface() const;
	graphics::texture write() const;
	
	void render_svg(const std::string& fname, int w, int h);
	void write_png(const std::string& fname);

	void set_pattern(cairo_pattern_t* pattern, bool take_ownership=true);

	float width() const { return width_; }
	float height() const { return height_; }
private:
	cairo_context(const cairo_context&);
	void operator=(const cairo_context&);

	cairo_surface_t* surface_;
	cairo_t* cairo_;
	int width_, height_;

	cairo_pattern_t* temp_pattern_;
};

struct cairo_matrix_saver
{
	cairo_matrix_saver(cairo_context& ctx);
	~cairo_matrix_saver();

	cairo_context& ctx_;
};

class cairo_callable : public game_logic::formula_callable
{
public:
	cairo_callable();
private:
	DECLARE_CALLABLE(cairo_callable);
};

namespace cairo_font
{

graphics::texture render_text_uncached(const std::string& text,
                                       const SDL_Color& color, int size, const std::string& font_name="");

int char_width(int size, const std::string& fn="");
int char_height(int size, const std::string& fn="");


}

}

#endif
