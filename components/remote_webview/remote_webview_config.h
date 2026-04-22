#pragma once

namespace esphome {
namespace remote_webview {
namespace cfg {

inline constexpr int decode_task_stack = 64 * 1024;
inline constexpr int ws_task_stack = 16 * 1024;
inline constexpr int ws_task_prio = 5;
inline constexpr int decode_queue_depth = 16;

inline constexpr size_t ws_max_message_bytes = 64 * 1024;
inline constexpr size_t ws_buffer_size = 30 * 1024;
inline constexpr size_t ws_keepalive_interval_us = 30 * 1000 * 1000;

inline constexpr bool coalesce_moves = true;
inline constexpr uint32_t move_rate_hz = 60;

} // namespace cfg
} // namespace remote_webview
} // namespace esphome
