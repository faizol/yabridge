// yabridge: a Wine VST bridge
// Copyright (C) 2020  Robbert van der Helm
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

#include "vst3.h"

#include "src/common/serialization/vst3.h"

Vst3Logger::Vst3Logger(Logger& generic_logger) : logger(generic_logger) {}

void Vst3Logger::log_unknown_interface(
    const std::string& where,
    const std::optional<Steinberg::FUID>& uid) {
    if (BOOST_UNLIKELY(logger.verbosity >= Logger::Verbosity::most_events)) {
        char uid_string[128] = "<invalid_pointer>";
        if (uid) {
            uid->print(uid_string, Steinberg::FUID::UIDPrintStyle::kCLASS_UID);
        }

        std::ostringstream message;
        message << "[unknown interface] " << where << ": " << uid_string;

        log(message.str());
    }
}

void Vst3Logger::log_request(bool is_host_vst, const YaComponent::Construct&) {
    log_request_base(is_host_vst, [](auto& message) {
        // TODO: Log the CID on verbosity level 2, and then also report all CIDs
        //       in the plugin factory
        message << "IPluginFactory::createComponent(cid = ..., _iid = "
                   "IComponent::iid, "
                   "&obj)";
    });
}

void Vst3Logger::log_request(bool is_host_vst,
                             const YaComponent::Destruct& request) {
    log_request_base(is_host_vst, [&](auto& message) {
        message << "<IComponent* #" << request.instance_id
                << ">::~IComponent()";
    });
}

void Vst3Logger::log_request(bool is_host_vst,
                             const YaComponent::Initialize& request) {
    log_request_base(is_host_vst, [&](auto& message) {
        message << "<IComponent* #" << request.instance_id
                << ">::initialize(context = ";
        if (request.host_application_context_args) {
            message << "<IHostApplication*>";
        } else {
            message << "<nullptr>";
        }
        message << ")";
    });
}

void Vst3Logger::log_request(bool is_host_vst,
                             const YaComponent::Terminate& request) {
    log_request_base(is_host_vst, [&](auto& message) {
        message << "<IComponent* #" << request.instance_id << ">::terminate()";
    });
}

void Vst3Logger::log_request(bool is_host_vst,
                             const YaComponent::SetIoMode& request) {
    log_request_base(is_host_vst, [&](auto& message) {
        message << "<IComponent* #" << request.instance_id
                << ">::setIoMode(mode = " << request.mode << ")";
    });
}

void Vst3Logger::log_request(bool is_host_vst,
                             const YaComponent::GetBusCount& request) {
    log_request_base(is_host_vst, [&](auto& message) {
        message << "<IComponent* #" << request.instance_id
                << ">::getBusCount(type = " << request.type
                << ", dir = " << request.dir << ")";
    });
}

void Vst3Logger::log_request(bool is_host_vst,
                             const YaComponent::GetBusInfo& request) {
    log_request_base(is_host_vst, [&](auto& message) {
        message << "<IComponent* #" << request.instance_id
                << ">::getBusInfo(type = " << request.type
                << ", dir = " << request.dir << ", index = " << request.index
                << ", &bus)";
    });
}

void Vst3Logger::log_request(bool is_host_vst,
                             const YaComponent::GetRoutingInfo& request) {
    log_request_base(is_host_vst, [&](auto& message) {
        message << "<IComponent* #" << request.instance_id
                << ">::getRoutingInfo(inInfo = <RoutingInfo& for bus "
                << request.in_info.busIndex << " and channel "
                << request.in_info.channel
                << ">, outInfo = <RoutingInfo& for bus "
                << request.out_info.busIndex << " and channel "
                << request.out_info.channel << ">)";
    });
}

void Vst3Logger::log_request(bool is_host_vst,
                             const YaComponent::ActivateBus& request) {
    log_request_base(is_host_vst, [&](auto& message) {
        message << "<IComponent* #" << request.instance_id
                << ">::activateBus(type = " << request.type
                << ", dir = " << request.dir << ", index = " << request.index
                << ", state = " << (request.state ? "true" : "false") << ")";
    });
}

void Vst3Logger::log_request(bool is_host_vst,
                             const YaComponent::SetActive& request) {
    log_request_base(is_host_vst, [&](auto& message) {
        message << "<IComponent* #" << request.instance_id
                << ">::setActive(state = " << (request.state ? "true" : "false")
                << ")";
    });
}

void Vst3Logger::log_request(bool is_host_vst,
                             const YaComponent::SetState& request) {
    log_request_base(is_host_vst, [&](auto& message) {
        message << "<IComponent* #" << request.instance_id
                << ">::setState(state = <IBStream* containing "
                << request.state.size() << "bytes>)";
    });
}

void Vst3Logger::log_request(bool is_host_vst,
                             const YaComponent::GetState& request) {
    log_request_base(is_host_vst, [&](auto& message) {
        message << "<IComponent* #" << request.instance_id
                << ">::getState(state = <IBStream*>)";
    });
}

void Vst3Logger::log_request(bool is_host_vst,
                             const YaComponent::SetBusArrangements& request) {
    log_request_base(is_host_vst, [&](auto& message) {
        message << "<IAudioProcessor* #" << request.instance_id
                << ">::setBusArrangements(inputs = [SpeakerArrangement; "
                << request.inputs.size() << "], numIns = " << request.num_ins
                << ", outputs = [SpeakerArrangement; " << request.outputs.size()
                << "], numOuts = " << request.num_outs << ")";
    });
}

void Vst3Logger::log_request(bool is_host_vst,
                             const YaComponent::GetBusArrangement& request) {
    log_request_base(is_host_vst, [&](auto& message) {
        message << "<IAudioProcessor* #" << request.instance_id
                << ">::getBusArrangement(dir = " << request.dir
                << ", index = " << request.index << ", &arr)";
    });
}

void Vst3Logger::log_request(bool is_host_vst,
                             const YaComponent::CanProcessSampleSize& request) {
    log_request_base(is_host_vst, [&](auto& message) {
        message << "<IAudioProcessor* #" << request.instance_id
                << ">::canProcessSampleSize(symbolicSampleSize = "
                << request.symbolic_sample_size << ")";
    });
}

void Vst3Logger::log_request(bool is_host_vst,
                             const YaPluginFactory::Construct&) {
    log_request_base(is_host_vst,
                     [](auto& message) { message << "GetPluginFactory()"; });
}

void Vst3Logger::log_request(bool is_host_vst,
                             const YaPluginFactory::SetHostContext&) {
    log_request_base(is_host_vst, [](auto& message) {
        message << "IPluginFactory3::setHostContext(IHostApplication*)";
    });
}

void Vst3Logger::log_request(bool is_host_vst, const WantsConfiguration&) {
    log_request_base(is_host_vst, [](auto& message) {
        message << "Requesting <Configuration>";
    });
}

void Vst3Logger::log_response(bool is_host_vst, const Ack&) {
    log_response_base(is_host_vst, [&](auto& message) { message << "ACK"; });
}

void Vst3Logger::log_response(
    bool is_host_vst,
    const std::variant<YaComponent::ConstructArgs, UniversalTResult>& result) {
    log_response_base(is_host_vst, [&](auto& message) {
        std::visit(overload{[&](const YaComponent::ConstructArgs& args) {
                                message << "<IComponent* #" << args.instance_id
                                        << ">";
                            },
                            [&](const UniversalTResult& code) {
                                message << code.string();
                            }},
                   result);
    });
}

void Vst3Logger::log_response(bool is_host_vst,
                              const YaComponent::GetBusInfoResponse& response) {
    log_response_base(is_host_vst, [&](auto& message) {
        message << response.result.string();
        if (response.result.native() == Steinberg::kResultOk) {
            message << ", <BusInfo>";
        }
    });
}

void Vst3Logger::log_response(
    bool is_host_vst,
    const YaComponent::GetRoutingInfoResponse& response) {
    log_response_base(is_host_vst, [&](auto& message) {
        message << response.result.string();
        if (response.result.native() == Steinberg::kResultOk) {
            message << ", <RoutingInfo& for bus "
                    << response.updated_in_info.busIndex << " and channel "
                    << response.updated_in_info.channel
                    << ", <RoutingInfo& for bus "
                    << response.updated_out_info.busIndex << " and channel "
                    << response.updated_out_info.channel << ">";
        }
    });
}

void Vst3Logger::log_response(bool is_host_vst,
                              const YaComponent::GetStateResponse& response) {
    log_response_base(is_host_vst, [&](auto& message) {
        message << response.result.string();
        if (response.result.native() == Steinberg::kResultOk) {
            message << ", <IBStream* containing "
                    << response.updated_state.size() << " bytes>";
        }
    });
}

void Vst3Logger::log_response(
    bool is_host_vst,
    const YaComponent::GetBusArrangementResponse& response) {
    log_response_base(is_host_vst, [&](auto& message) {
        message << response.result.string();
        if (response.result.native() == Steinberg::kResultOk) {
            message << ", <SpeakerArrangement>";
        }
    });
}

void Vst3Logger::log_response(bool is_host_vst,
                              const YaPluginFactory::ConstructArgs& args) {
    log_response_base(is_host_vst, [&](auto& message) {
        message << "<IPluginFactory*> with " << args.num_classes
                << " registered classes";
    });
}

void Vst3Logger::log_response(bool is_host_vst, const Configuration&) {
    log_response_base(is_host_vst,
                      [](auto& message) { message << "<Configuration"; });
}
