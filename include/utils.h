#include <nta/PrimitiveBatch.h>

extern void render_line_in_pieces(nta::PrimitiveBatch& pbatch, glm::vec2 start, glm::vec2 end,
                                  int num_pieces, glm::vec4 color = glm::vec4(1));