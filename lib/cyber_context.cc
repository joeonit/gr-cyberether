/* -*- c++ -*- */
/*
 * Copyright 2026 Youssef Mahmoud.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/cyberether/cyber_context.h>
#include <jetstream/logger.hh>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <string>
#include <thread>
#include <unordered_set>

namespace gr {
  namespace cyberether {
    using namespace Jetstream;

    cyber_context&
    cyber_context::instance()  // singleton instance
    {
      static cyber_context ctx;
      return ctx;
    }

    void
    cyber_context::register_plot(const plot_request& request)
    {
      std::lock_guard<std::mutex> lock(d_mutex);
      auto it = std::find_if(d_plots.begin(), d_plots.end(),
                             [&](const plot_request& p) { return p.owner == request.owner; });
      if (it != d_plots.end()) {
          *it = request;
      } else {
          d_plots.push_back(request);
      }
    }

    void
    cyber_context::unregister_plot(const void* owner)
    {
      std::lock_guard<std::mutex> lock(d_mutex);
      d_plots.erase(std::remove_if(d_plots.begin(), d_plots.end(),
                                   [&](const plot_request& p) { return p.owner == owner; }),
                    d_plots.end());
    }


    void
    cyber_context::present()
    {
      if (d_started.exchange(true, std::memory_order_acq_rel)) {
          JST_WARN("[gr-cyberether] present() is already running; ignoring re-entry.");
          return;
      }

      // Snapshot the registered plots: present() blocks for the whole lifetime
      // of the window, so we must not hold the lock across it.
      std::vector<plot_request> plots;
      {
          std::lock_guard<std::mutex> lock(d_mutex);
          plots = d_plots;
      }

      if (plots.empty()) {
          JST_WARN("[gr-cyberether] present() called but no CyberEther sinks are "
                   "registered; nothing to show.");
          return;
      }

      // square grid: cols = ceil(sqrt(n)), rows = ceil(n / cols).
      //   1 -> 1x1   2 -> 1x2   3,4 -> 2x2   5,6 -> 2x3   ...
      const std::size_t n    = plots.size();
      const U8          cols = static_cast<U8>(std::ceil(std::sqrt(static_cast<double>(n))));
      const U8          rows = static_cast<U8>((n + cols - 1) / cols);


      std::unordered_set<std::string> used_names;

      for (std::size_t i = 0; i < n; ++i) {
          const U8 row = static_cast<U8>(i / cols);
          const U8 col = static_cast<U8>(i % cols);
          const auto mosaic = Superluminal::MosaicLayout(rows, cols, 1, 1, col, row);

          std::string name = plots[i].name;
          if (!used_names.insert(name).second) {
              const std::string original = name;
              for (int k = 2; !used_names.insert(name = original + " (" + std::to_string(k) + ")").second; ++k) {
              }
              JST_WARN("[gr-cyberether] duplicate plot name '{}'; showing it as '{}'. "
                       "Give CyberEther sinks unique names.", original, name);
          }

          const Result res = Superluminal::Plot(name, mosaic, plots[i].config);
          if (res != Result::SUCCESS) {
              JST_FATAL("[gr-cyberether] Superluminal::Plot failed for '{}'.", name);
              Superluminal::Terminate();
              return;
          }
      }

      JST_INFO("[gr-cyberether] Presenting {} plot(s) in a {}x{} grid.", n, rows, cols);

      // Drive Superluminal::compute() at display cadence on a single background
      // thread, matching Superluminal::Show(). Sinks only write their tensors
      // from GR work(); they never call Update() themselves.
      std::atomic<bool> update_running{false};
      std::thread       update_thread;

      try {
          if (Superluminal::Start() != Result::SUCCESS) {
              JST_FATAL("[gr-cyberether] Superluminal::Start failed.");
              d_started.store(false, std::memory_order_release);
              Superluminal::Terminate();
              return;
          }

          update_running.store(true, std::memory_order_release);
          update_thread = std::thread([&update_running]() {
              while (update_running.load(std::memory_order_acquire) &&
                     Superluminal::Presenting()) {
                  Superluminal::Update();
                  std::this_thread::sleep_for(std::chrono::milliseconds(16));
              }
          });

          Superluminal::Block();   // main-thread event loop until window closes
      } catch (const std::exception& e) {
          JST_ERROR("[gr-cyberether] exception while presenting: {}", e.what());
      } catch (...) {
          JST_ERROR("[gr-cyberether] unknown exception while presenting.");
      }

      update_running.store(false, std::memory_order_release);
      if (update_thread.joinable()) {
          update_thread.join();
      }

      d_started.store(false, std::memory_order_release);

      // Stop()/Terminate() are safe even if Start() failed (they no-op when not
      // running / not initialized), so the instance is always torn down here
      // never left initialized to crash at process exit.
      Superluminal::Stop();
      Superluminal::Terminate();
    }

    void
    present()
    {
      cyber_context::instance().present();
    }

  } // namespace cyberether
} // namespace gr
