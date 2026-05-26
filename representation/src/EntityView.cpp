#include "representation/EntityView.h"

namespace representation {

EntityView::EntityView(std::shared_ptr<sf::RenderWindow> window) : m_window{std::move(window)} {}

}  // namespace representation
