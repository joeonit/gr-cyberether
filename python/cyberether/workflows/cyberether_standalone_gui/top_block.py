"""Generator for Python flow graphs that open a CyberEther window.

The generated main() instantiates the top block, then calls
``cyberether.present(tb)`` on the main thread, which:

  * starts the flowgraph,
  * opens the CyberEther window (Superluminal Block() event loop),
  * blocks the main thread until the window is closed,
  * stops and waits the flowgraph on teardown.

SPDX-License-Identifier: GPL-3.0-or-later
"""

import os

from gnuradio.grc.workflows.common import PythonGeneratorBase
from gnuradio.grc.core.Constants import TOP_BLOCK_FILE_MODE


class CyberetherStandaloneGuiGenerator(PythonGeneratorBase):
    """Generator for Python flow graphs with a CyberEther main loop."""

    def __init__(self, flow_graph, output_dir):
        super().__init__(
            flow_graph,
            output_dir,
            add_xterm=False,
            py_template=os.path.join(
                os.path.dirname(__file__),
                "flow_graph_cyberether.py.mako"))
        self._mode = TOP_BLOCK_FILE_MODE

    def write(self, _=None):
        self._warnings()

        fg = self._flow_graph
        self.title = fg.get_option('title') or fg.get_option(
            'id').replace('_', ' ').title()

        for filename, data in self._build_python_code_from_template():
            with open(filename, 'w', encoding='utf-8') as fp:
                fp.write(data)
            if filename == self.file_path:
                os.chmod(filename, self._mode)
