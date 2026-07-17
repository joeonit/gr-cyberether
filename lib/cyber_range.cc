/* -*- c++ -*- */
/*
 * Copyright 2026 Youssef Mahmoud.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/cyberether/cyber_range.h>
#include <gnuradio/cyberether/cyber_context.h>
#include <jetstream/logger.hh>
#include <jetstream/superluminal.hh>

#include <atomic>
#include <mutex>

namespace gr {
  namespace cyberether {
    using namespace Jetstream;

    class cyber_range_impl : public cyber_range
    {
     private:
      const std::string d_label;
      const float       d_min;
      const float       d_max;
      float             d_slider;   // bound to the ImGui slider; present thread only
      float             d_last;
      std::atomic<float> d_value;   // last applied value, for readers
      std::function<void(float)> d_callback;
      std::mutex        d_mutex;

      // Runs every frame on the present thread.
      void draw()
      {
        Superluminal::Slider(d_label, d_min, d_max, d_slider);
        if (d_slider != d_last) {
            d_last = d_slider;
            d_value.store(d_slider, std::memory_order_relaxed);
            std::lock_guard<std::mutex> lock(d_mutex);
            if (d_callback) {
                d_callback(d_slider);
            }
        }
      }

     public:
      cyber_range_impl(const std::string& label, float min, float max,
                       float value, const std::string& gui_hint)
        : d_label(label), d_min(min), d_max(max),
          d_slider(value), d_last(value), d_value(value)
      {
        cyber_context::plot_request request{ this, d_label, {}, gui_hint };
        request.interface = [this] { draw(); };
        cyber_context::instance().register_plot(request);

        JST_INFO("[gr-cyberether] cyber_range '{}' constructed: [{}, {}] = {}.",
                 d_label, d_min, d_max, d_slider);
      }

      ~cyber_range_impl() override
      {
        cyber_context::instance().unregister_plot(this);
      }

      void set_callback(std::function<void(float)> callback) override
      {
        std::lock_guard<std::mutex> lock(d_mutex);
        d_callback = std::move(callback);
      }

      float value() const override
      {
        return d_value.load(std::memory_order_relaxed);
      }
    };

    cyber_range::sptr
    cyber_range::make(const std::string& label, float min, float max,
                      float value, const std::string& gui_hint)
    {
      return std::make_shared<cyber_range_impl>(label, min, max, value, gui_hint);
    }

  } /* namespace cyberether */
} /* namespace gr */
