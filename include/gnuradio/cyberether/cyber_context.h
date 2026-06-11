#ifndef INCLUDED_CYBERETHER_CYBER_CONTEXT_H
#define INCLUDED_CYBERETHER_CYBER_CONTEXT_H

#include <gnuradio/cyberether/api.h>
#include <jetstream/superluminal.hh>

namespace gr {
  namespace cyberether {

    class CYBERETHER_API cyber_context
    {
    public:
      cyber_context()
      {
        Jetstream::Superluminal::Initialize();
      }

      ~cyber_context()
      {
        Jetstream::Superluminal::Terminate();
      }

      cyber_context(const cyber_context&)            = delete;
      cyber_context& operator=(const cyber_context&) = delete;
      cyber_context(cyber_context&&)                 = delete;
      cyber_context& operator=(cyber_context&&)      = delete;

      // Start Superluminal's compute + present threads,
      // run the windowing event loop on the calling thread, then unwind.
      void run()
      {
        Jetstream::Superluminal::Start();
        Jetstream::Superluminal::Block();
        Jetstream::Superluminal::Stop();
      }

      // Whether a window is currently open. Forwards
      // Superluminal::Presenting().

      bool is_presenting() const
      {
        return Jetstream::Superluminal::Presenting();
      }
    };

  } // namespace cyberether
} // namespace gr

#endif /* INCLUDED_CYBERETHER_CYBER_CONTEXT_H */
