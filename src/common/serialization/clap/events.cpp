// yabridge: a Wine plugin bridge
// Copyright (C) 2020-2022 Robbert van der Helm
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "events.h"

#include "../../utils.h"

namespace clap {
namespace events {

std::optional<Event> Event::parse(const clap_event_header_t& generic_event) {
    std::optional<decltype(Event::payload)> payload;
    if (generic_event.space_id == CLAP_CORE_EVENT_SPACE_ID) {
        switch (generic_event.type) {
            case CLAP_EVENT_NOTE_ON:
            case CLAP_EVENT_NOTE_OFF:
            case CLAP_EVENT_NOTE_CHOKE:
            case CLAP_EVENT_NOTE_END: {
                const auto& event =
                    reinterpret_cast<const clap_event_note_t&>(generic_event);

                // The original event type can be restored from the header
                payload = payload::Note{.event = event};
            } break;
            case CLAP_EVENT_NOTE_EXPRESSION: {
                const auto& event =
                    reinterpret_cast<const clap_event_note_expression_t&>(
                        generic_event);
                payload = payload::NoteExpression{.event = event};
            } break;
            case CLAP_EVENT_PARAM_VALUE: {
                const auto& event =
                    reinterpret_cast<const clap_event_param_value_t&>(
                        generic_event);
                payload = payload::ParamValue{.event = event};
            } break;
            case CLAP_EVENT_PARAM_MOD: {
                const auto& event =
                    reinterpret_cast<const clap_event_param_mod_t&>(
                        generic_event);
                payload = payload::ParamMod{.event = event};
            } break;
            case CLAP_EVENT_PARAM_GESTURE_BEGIN:
            case CLAP_EVENT_PARAM_GESTURE_END: {
                const auto& event =
                    reinterpret_cast<const clap_event_param_gesture_t&>(
                        generic_event);
                payload = payload::ParamGesture{.event = event};
            } break;
            case CLAP_EVENT_TRANSPORT: {
                const auto& event =
                    reinterpret_cast<const clap_event_transport_t&>(
                        generic_event);
                payload = payload::Transport{.event = event};
            } break;
            case CLAP_EVENT_MIDI: {
                const auto& event =
                    reinterpret_cast<const clap_event_midi_t&>(generic_event);
                payload = payload::Midi{.event = event};
            } break;
            case CLAP_EVENT_MIDI_SYSEX: {
                const auto& event =
                    reinterpret_cast<const clap_event_midi_sysex_t&>(
                        generic_event);

                assert(event.buffer);
                const auto sysex_payload = payload::MidiSysex{
                    .event =
                        clap_event_midi_sysex_t{
                            .header = event.header,
                            .port_index = event.port_index,
                            // The buffer and size fields will be restored
                            // during the `get_header()` call. Nulling the
                            // pointer and zeroing the size should make
                            // incorrect usage much easier to spot than leaving
                            // them dangling.
                            .buffer = nullptr,
                            .size = 0},
                    .buffer =
                        std::string(reinterpret_cast<const char*>(event.buffer),
                                    event.size)};
            } break;
            case CLAP_EVENT_MIDI2: {
                const auto& event =
                    reinterpret_cast<const clap_event_midi2_t&>(generic_event);
                payload = payload::Midi2{.event = event};
            } break;
        }
    }

    if (payload) {
        return Event{.payload = std::move(*payload)};
    } else {
        return std::nullopt;
    }
}

const clap_event_header_t* Event::get() const {
    return std::visit(
        overload{[](payload::MidiSysex& event) -> const clap_event_header_t* {
                     // These events contain heap data pointers. We store this
                     // data using an `std::string` alongside the event struct,
                     // but we can only set the pointer here just before
                     // returning the event in case it was moved inbetween
                     // deserialization and this function being called.
                     event.event.buffer =
                         reinterpret_cast<const uint8_t*>(event.buffer.data());
                     event.event.size = event.buffer.size();

                     return &event.event.header;
                 },
                 [](const auto& event) -> const clap_event_header_t* {
                     return &event.event.header;
                 }},
        payload);
}

}  // namespace events
}  // namespace clap