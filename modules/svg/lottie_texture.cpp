/**************************************************************************/
/*  lottie_texture.cpp                                                    */
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

#include "lottie_texture.h"

#include "core/os/memory.h"
#include "core/variant/variant.h"

#include <thorvg.h>

void LottieTexture2D::_update_image(float p_scale) {
	if (origin_width < 0 && origin_height < 0) {
		float fw, fh;
		picture->size(&fw, &fh);
		origin_width = fw;
		origin_height = fh;
	}
	uint32_t w = MAX(1, round(origin_width * p_scale));
	uint32_t h = MAX(1, round(origin_height * p_scale));

	const uint32_t max_dimension = 16384;
	if (w > max_dimension || h > max_dimension) {
		WARN_PRINT(vformat(
				String::utf8("LottieTexture2D: Target canvas dimensions %d×%d (with scale %.2f) exceed the max supported dimensions %d×%d. The target canvas will be scaled down."),
				w, h, p_scale, max_dimension, max_dimension));
		w = MIN(w, max_dimension);
		h = MIN(h, max_dimension);
	}

	picture->size(w, h);
	image = Image::create_empty(w, h, false, Image::FORMAT_RGBA8);
	// Note: memalloc here, be sure to memfree before any return.
	buffer = (uint32_t *)(buffer == nullptr ? memalloc(sizeof(uint32_t) * w * h) : memrealloc(buffer, sizeof(uint32_t) * w * h));
	// Update the image to current frame.
	set_frame(frame);
}

Ref<LottieTexture2D> LottieTexture2D::create_from_json(Ref<JSON> p_json, float p_scale) {
	Ref<LottieTexture2D> ret = memnew(LottieTexture2D);
	ret->set_json(p_json);
	ret->set_scale(p_scale);
	return ret;
}

Ref<LottieTexture2D> LottieTexture2D::create_from_string(String p_string, float p_scale) {
	Ref<LottieTexture2D> ret = memnew(LottieTexture2D);
	Ref<JSON> data = memnew(JSON);
	Error res = data->parse(p_string, true);
	ERR_FAIL_COND_V_MSG(res != OK, nullptr, "LottieTexture2D: Parse JSON failed.");
	ret->set_json(data);
	ret->set_scale(p_scale);
	return ret;
}

void LottieTexture2D::set_frame(float _frame) {
	if (image.is_null()) {
		return;
	}
	tvg::Result res = animation->frame(_frame);
	if (texture.is_null() || res == tvg::Result::Success) {
		sw_canvas->update(picture);
	}

	uint32_t width = image->get_width();
	uint32_t height = image->get_height();
	res = sw_canvas->target(buffer, width, width, height, tvg::SwCanvas::ARGB8888S);
	if (res != tvg::Result::Success) {
		ERR_FAIL_MSG("LottieTexture2D: Couldn't set target on ThorVG canvas.");
	}

	res = sw_canvas->push(tvg::cast(picture));
	if (res != tvg::Result::Success) {
		ERR_FAIL_MSG("LottieTexture2D: Couldn't insert ThorVG picture on canvas.");
	}

	res = sw_canvas->draw();
	if (res != tvg::Result::Success) {
		ERR_FAIL_MSG("LottieTexture2D: Couldn't draw ThorVG pictures on canvas.");
	}

	res = sw_canvas->sync();
	if (res != tvg::Result::Success) {
		ERR_FAIL_MSG("LottieTexture2D: Couldn't sync ThorVG canvas.");
	}

	Vector<uint8_t> image_data;
	image_data.resize(width * height * sizeof(uint32_t));

	for (uint32_t y = 0; y < height; y++) {
		for (uint32_t x = 0; x < width; x++) {
			uint32_t n = buffer[y * width + x];
			const size_t offset = sizeof(uint32_t) * width * y + sizeof(uint32_t) * x;
			image_data.write[offset + 0] = (n >> 16) & 0xff;
			image_data.write[offset + 1] = (n >> 8) & 0xff;
			image_data.write[offset + 2] = n & 0xff;
			image_data.write[offset + 3] = (n >> 24) & 0xff;
		}
	}

	res = sw_canvas->clear(true);

	image->set_data(width, height, false, Image::FORMAT_RGBA8, image_data);

	if (texture.is_null()) {
		texture = RenderingServer::get_singleton()->texture_2d_create(image);
	} else {
		RID new_texture = RenderingServer::get_singleton()->texture_2d_create(image);
		RenderingServer::get_singleton()->texture_replace(texture, new_texture);
	}
	frame = _frame;
	emit_changed();
}

float LottieTexture2D::get_frame_count() { return animation->totalFrame(); };

float LottieTexture2D::get_duration() { return animation->duration(); };

void LottieTexture2D::set_json(Ref<JSON> p_json) {
	if (p_json.is_null()) {
		return;
	}
	String data = p_json->get_parsed_text();
	if (data.is_empty()) {
		// don't sort keys, otherwise ThorVG can't load it
		data = JSON::stringify(p_json->get_data(), "", false);
	}
	tvg::Result result = picture->load(data.utf8(), data.utf8().size(), "lottie", true);
	if (result != tvg::Result::Success) {
		ERR_FAIL_MSG(vformat("LottieTexture2D: Couldn't load Lottie: %s.",
				result == tvg::Result::InvalidArguments				   ? "InvalidArguments"
						: result == tvg::Result::NonSupport			   ? "NonSupport"
						: result == tvg::Result::InsufficientCondition ? "InsufficientCondition"
																	   : "Unknown Error"));
	}
	_update_image(scale);
	json = p_json;
	emit_changed();
}

void LottieTexture2D::set_scale(float p_scale) {
	ERR_FAIL_COND_MSG(Math::is_zero_approx(p_scale), "LottieTexture2D: Can't load Lottie with a scale of 0.");
	_update_image(p_scale);
	scale = p_scale;
	emit_changed();
};

RID LottieTexture2D::get_rid() const {
	if (texture.is_null()) {
		texture = RenderingServer::get_singleton()->texture_2d_placeholder_create();
	}
	return texture;
}

LottieTexture2D::~LottieTexture2D() {
	if (texture.is_valid()) {
		RenderingServer::get_singleton()->free(texture);
	}
	if (buffer) {
		memfree(buffer);
	}
}

void LottieTexture2D::_bind_methods() {
	ClassDB::bind_static_method("LottieTexture2D", D_METHOD("create_from_string", "p_string", "p_scale"), &LottieTexture2D::create_from_string, DEFVAL(1));
	ClassDB::bind_static_method("LottieTexture2D", D_METHOD("create_from_json", "p_json", "p_scale"), &LottieTexture2D::create_from_json, DEFVAL(1));
	ClassDB::bind_method(D_METHOD("set_json", "p_json"), &LottieTexture2D::set_json);
	ClassDB::bind_method(D_METHOD("get_json"), &LottieTexture2D::get_json);
	ClassDB::bind_method(D_METHOD("set_scale", "p_scale"), &LottieTexture2D::set_scale);
	ClassDB::bind_method(D_METHOD("get_scale"), &LottieTexture2D::get_scale);
	ClassDB::bind_method(D_METHOD("set_frame", "frame"), &LottieTexture2D::set_frame);
	ClassDB::bind_method(D_METHOD("get_frame"), &LottieTexture2D::get_frame);
	ClassDB::bind_method(D_METHOD("get_frame_count"), &LottieTexture2D::get_frame_count);
	ClassDB::bind_method(D_METHOD("get_duration"), &LottieTexture2D::get_duration);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "json", PROPERTY_HINT_RESOURCE_TYPE, "JSON"), "set_json", "get_json");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "scale"), "set_scale", "get_scale");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "frame"), "set_frame", "get_frame");
}
