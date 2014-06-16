/*
	Copyright (C) 2003-2013 by Kristina Simpson <sweet.kristas@gmail.com>
	
	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	   1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgement in the product documentation would be
	   appreciated but is not required.

	   2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.

	   3. This notice may not be removed or altered from any source
	   distribution.
*/

#include <tuple>

#include "Surface.hpp"

namespace KRE
{
	namespace
	{
		typedef std::map<std::string,std::tuple<SurfaceCreatorFileFn,SurfaceCreatorPixelsFn,SurfaceCreatorMaskFn>> CreatorMap;
		CreatorMap& get_surface_creator()
		{
			static CreatorMap res;
			return res;
		}

		typedef std::map<std::string, SurfacePtr> SurfaceCacheType;
		SurfaceCacheType& get_surface_cache()
		{
			static SurfaceCacheType res;
			return res;
		}
	}

	Surface::Surface()
	{
	}

	Surface::~Surface()
	{
	}

	PixelFormatPtr Surface::getPixelFormat()
	{
		return pf_;
	}

	void Surface::setPixelFormat(PixelFormatPtr pf)
	{
		pf_ = pf;
	}

	SurfaceLock::SurfaceLock(const SurfacePtr& surface)
		: surface_(surface)
	{
		surface_->lock();
	}

	SurfaceLock::~SurfaceLock()
	{
		surface_->unlock();
	}

	SurfacePtr Surface::convert(PixelFormat::PF fmt, SurfaceConvertFn convert)
	{
		return handleConvert(fmt, convert);
	}

	bool Surface::registerSurfaceCreator(const std::string& name, SurfaceCreatorFileFn file_fn, SurfaceCreatorPixelsFn pixels_fn, SurfaceCreatorMaskFn mask_fn)
	{
		return get_surface_creator().insert(std::make_pair(name,std::make_tuple(file_fn, pixels_fn, mask_fn))).second;
	}

	void Surface::unRegisterSurfaceCreator(const std::string& name)
	{
		auto it = get_surface_creator().find(name);
		ASSERT_LOG(it != get_surface_creator().end(), "Unable to find surface creator: " << name);
		get_surface_creator().erase(it);
	}

	SurfacePtr Surface::create(const std::string& filename, bool no_cache, PixelFormat::PF fmt, SurfaceConvertFn convert)
	{
		ASSERT_LOG(get_surface_creator().empty() == false, "No resources registered to create images from files.");
		auto create_fn_tuple = get_surface_creator().begin()->second;
		if(!no_cache) {
			auto it = get_surface_cache().find(filename);
			if(it != get_surface_cache().end()) {
				return it->second;
			}
			auto surface = std::get<0>(create_fn_tuple)(filename, fmt, convert);
			get_surface_cache()[filename] = surface;
			return surface;
		} 
		return std::get<0>(create_fn_tuple)(filename, fmt, convert);
	}

	SurfacePtr Surface::create(unsigned width, 
		unsigned height, 
		unsigned bpp, 
		unsigned row_pitch, 
		uint32_t rmask, 
		uint32_t gmask, 
		uint32_t bmask, 
		uint32_t amask, 
		const void* pixels)
	{
		// XXX no caching as default?
		ASSERT_LOG(get_surface_creator().empty() == false, "No resources registered to create images from files.");
		auto create_fn_tuple = get_surface_creator().begin()->second;
		return std::get<1>(create_fn_tuple)(width, height, bpp, row_pitch, rmask, gmask, bmask, amask, pixels);
	}

	SurfacePtr Surface::create(unsigned width, 
		unsigned height, 
		unsigned bpp, 
		uint32_t rmask, 
		uint32_t gmask, 
		uint32_t bmask, 
		uint32_t amask)
	{
		// XXX no caching as default?
		ASSERT_LOG(get_surface_creator().empty() == false, "No resources registered to create images from files.");
		auto create_fn_tuple = get_surface_creator().begin()->second;
		return std::get<2>(create_fn_tuple)(width, height, bpp, rmask, gmask, bmask, amask);
	}

	void Surface::resetSurfaceCache()
	{
		get_surface_cache().clear();
	}

	PixelFormat::PixelFormat()
	{
	}

	PixelFormat::~PixelFormat()
	{
	}
}
