/* -*- c++ -*- */
/*
 * Copyright 2026 Youssef Mahmoud.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_CYBERETHER_CYBER_CONTEXT_H
#define INCLUDED_CYBERETHER_CYBER_CONTEXT_H

#include <gnuradio/cyberether/api.h>
#include <jetstream/superluminal.hh>

#include <atomic>
#include <mutex>
#include <string>
#include <vector>

namespace gr {
  namespace cyberether {

    /*!
     * \brief Process-wide owner of the Superluminal lifecycle.
     *
     * Controls the lifecycle of the Superluminal window and plot registration.
     * Manages different plots and mosaic layouts.
     *
     * present() MUST run on the process main thread.
     */
    class CYBERETHER_API cyber_context
    {
    public:

      struct plot_request {
        const void* owner;
        std::string name;
        Jetstream::Superluminal::PlotConfig config;
      };

      static cyber_context& instance();

      //! Record a plot. Safe on a GR worker thread (no Superluminal calls).
      void register_plot(const plot_request& request);

      //! Forget a plot (called from a sink's stop()), keyed by owner.
      void unregister_plot(const void* owner);

      /*!
       * \brief Open the window on the calling thread and block until it closes.
       *
       * Lays every registered plot out in a square grid, then runs the full
       * Superluminal lifecycle. Must be called on the main thread.
       */
      void present();


      cyber_context(const cyber_context&)            = delete;
      cyber_context& operator=(const cyber_context&) = delete;

    private:
      cyber_context()  = default;
      ~cyber_context() = default;

      mutable std::mutex        d_mutex;
      std::vector<plot_request> d_plots;        // registration order == grid order
      std::atomic<bool>         d_started{false}; // true only between Start()/Stop()
    };

    /*!
     * \brief Open the CyberEther window (convenience entry point).
     */
    CYBERETHER_API void present();

  } // namespace cyberether
} // namespace gr

#endif /* INCLUDED_CYBERETHER_CYBER_CONTEXT_H */
