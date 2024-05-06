/**************************************************************************/
/*  lottie_texture.h                                                      */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#ifndef LOTTIE_TEXTURE_H
#define LOTTIE_TEXTURE_H

#include "core/io/json.h"
#include "scene/resources/texture.h"

#include <thorvg.h>

class LottieTexture2D : public Texture2D {
	GDCLASS(LottieTexture2D, Texture2D);

	std::unique_ptr<tvg::SwCanvas> sw_canvas = tvg::SwCanvas::gen();
	std::unique_ptr<tvg::Animation> animation = tvg::Animation::gen();
	tvg::Picture *picture = animation->picture();
	Ref<Image> image;
	mutable RID texture;
	uint32_t *buffer = nullptr;
	Ref<JSON> json;

	float scale = 1.0;
	float origin_width = -1, origin_height = -1;
	float frame = 0;

	void _update_image(float p_scale);

protected:
	static void _bind_methods();

public:
	static Ref<LottieTexture2D> create_from_string(String p_string, float p_scale = 1);
	static Ref<LottieTexture2D> create_from_json(Ref<JSON> p_json, float p_scale = 1);

	void set_json(Ref<JSON> p_json);
	Ref<JSON> get_json() { return json; };

	void set_scale(float p_scale);
	float get_scale() { return scale; };

	void set_frame(float frame);
	float get_frame() { return frame; };

	float get_total_frame();
	float get_duration();

	int get_width() const override { return image->get_width(); };
	int get_height() const override { return image->get_height(); };
	Size2 get_size() const override { return image->get_size(); };
	virtual bool is_pixel_opaque(int p_x, int p_y) const override { return image.is_valid() ? image->get_pixel(p_x, p_y).a > 0.1 : true; };
	virtual bool has_alpha() const override { return true; };
	virtual Ref<Image> get_image() const override { return image; };
	virtual RID get_rid() const override;

	~LottieTexture2D();
};

#endif // LOTTIE_TEXTURE_H
