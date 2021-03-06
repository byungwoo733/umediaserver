// Copyright (c) 2015-2018 LG Electronics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef __MDC_HFSM_ICONNECCTOR_H__
#define __MDC_HFSM_ICONNECCTOR_H__

#include <string>
#include <list>
#include <functional>
#include <memory>
#include "dto_types.h"

// TODO: implement proper logging engine. cant rely on PmLogLib in all cases
// #define LOG_DEBUG(l, f, ...) fprintf(stderr, f, ##__VA_ARGS__); fprintf(stderr, "\n")

namespace uMediaServer { namespace mdc {

struct ITVDisplay {
	enum {
		AUDIO_CHANNEL = 1 << 0,
		VIDEO_CHANNEL = 1 << 1
	};
	enum class SubOverlayMode {
		NONE = 0,
		PBP
	};
	typedef std::function<void(const std::string &, bool)> callback_t;
	// VSM regestration result callback
	virtual void vsm_set_registration_callback(callback_t &&) = 0;
	// VSM connection state observer
	virtual void vsm_set_connection_observer(callback_t &&) = 0;

	virtual void vsm_register(const std::string & id, const res_t & adec, const res_t & vdec) = 0;
	virtual void vsm_unregister(const std::string & id) = 0;
	virtual void vsm_connect(const std::string & id, sink_t sink) = 0;
	virtual void vsm_disconnect(const std::string & id) = 0;

	virtual void avblock_mute(const std::string & id, size_t channel) = 0;
	virtual void avblock_unmute(const std::string & id, size_t channel) = 0;
	// AVBLOCK video mute state callback: true <=> muted; false <=> unmuted
	virtual void avblock_set_muted_callback(callback_t &&) = 0;

	virtual void display_set_window(const std::string & id, const display_out_t & display_out) = 0;
	virtual void display_set_video_info(const std::string & id, const video_info_t & video_info) = 0;
	virtual void display_set_alpha(const std::string & id, double alpha) = 0;
	virtual void display_set_alpha(sink_t sink, double alpha) = 0;
	virtual void display_set_sub_overlay_mode(SubOverlayMode mode) = 0;
	// DISPLAY configuration completed callback
	virtual void display_set_config_completed_callback(callback_t &&) = 0;

	virtual void sound_connect(const std::string & id) = 0;
	virtual void sound_disconnect(const std::string & id) = 0;
	// DASS connection state observer
	virtual void sound_set_connection_observer(callback_t &&) = 0;

	virtual int32_t get_plane_id(const std::string &) const { return -1; }

	virtual ~ITVDisplay() = default;
};

struct IMediaObject {
	typedef std::shared_ptr<const IMediaObject> ptr_t;
	typedef std::weak_ptr<const IMediaObject> weak_ptr_t;

	virtual const std::string & id() const = 0;
	virtual const std::string & appId() const = 0;
	virtual bool foreground() const = 0;
	virtual bool focus() const = 0;
	virtual bool hasAudio() const = 0;
	virtual bool hasVideo() const = 0;
	virtual bool autoLayout() const = 0;
};

// Video / Audio channel connection manager
struct IChannelConnection {
	// try conect media element to channel
	virtual sink_t try_connect (IMediaObject::ptr_t media) = 0;
	// commit connection of media element
	virtual void commit_connect (IMediaObject::ptr_t media) = 0;
	// query current channel connection
	virtual sink_t connected (const std::string & id) const = 0;
	// query current channel connection request
	virtual sink_t requested (const std::string & id) const = 0;
	// notify lost channel connection
	virtual void disconnected (const std::string & id) = 0;
};

struct IConnectionPolicy {
	virtual IChannelConnection & audio() = 0;
	virtual const IChannelConnection & audio() const = 0;
	virtual IChannelConnection & video() = 0;
	virtual const IChannelConnection & video() const = 0;
	// TODO: remove methods when meaningful tv.sound conection observer would be available
	virtual IMediaObject::ptr_t connected(mdc::sink_t) const = 0;
	virtual IMediaObject::ptr_t requested(mdc::sink_t) const = 0;
};

// Interface to maintain mdc application blacklist
struct IAcbObserver {
	// check if given app is blacklisted
	// true if balcklisted, false otherwise
	virtual bool check_blacklist(const std::string & app) const = 0;
};

// Interface for video overlay layout manager
struct ILayoutManager {
	virtual display_out_t suggest_layout(const video_info_t & vi, const std::string & id) const = 0;
};

}} // namespace uMediaServer::mdc

#endif // __MDC_HFSM_ICONNECCTOR_H__
