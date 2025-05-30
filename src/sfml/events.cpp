#include "events.hpp"
namespace events
{
[[nodiscard]] sf_event_variant get(const sf::Event &event) noexcept
{
     switch (event.type)
     {
          case sf::Event::EventType::Closed:
               ///< The window requested to be closed (no data)
          case sf::Event::EventType::LostFocus:
               ///< The window lost the focus (no data)
          case sf::Event::EventType::GainedFocus:
               ///< The window gained the focus (no data)
          case sf::Event::EventType::MouseEntered:
               ///< The mouse cursor entered the area of the window (no data)
          case sf::Event::EventType::MouseLeft:
               ///< The mouse cursor left the area of the window (no data)
          case sf::Event::EventType::Count:
               ///< Keep last -- the total number of event types
          default:
               return std::monostate{};
          case sf::Event::EventType::Resized:
               ///< The window was resized (data in event.size)
               return event.size;
          case sf::Event::EventType::TextEntered:
               ///< A character was entered (data in event.text)
               return event.text;
          case sf::Event::EventType::KeyPressed:
               ///< A key was pressed (data in event.key)
          case sf::Event::EventType::KeyReleased:
               ///< A key was released (data in event.key)
               return event.key;
          case sf::Event::EventType::MouseWheelMoved:
               ///< The mouse wheel was scrolled (data in event.mouseWheel) (deprecated)
               return event.mouseWheel;
          case sf::Event::EventType::MouseWheelScrolled:
               ///< The mouse wheel was scrolled (data in event.mouseWheelScroll)
               return event.mouseWheelScroll;
          case sf::Event::EventType::MouseButtonPressed:
               ///< A mouse button was pressed (data in event.mouseButton)
          case sf::Event::EventType::MouseButtonReleased:
               ///< A mouse button was released (data in event.mouseButton)
               return event.mouseButton;
          case sf::Event::EventType::MouseMoved:
               ///< The mouse cursor moved (data in event.mouseMove)
               return event.mouseMove;
          case sf::Event::EventType::JoystickButtonPressed:
               ///< A joystick button was pressed (data in event.joystickButton)
          case sf::Event::EventType::JoystickButtonReleased:
               ///< A joystick button was released (data in event.joystickButton)
               return event.joystickButton;
          case sf::Event::EventType::JoystickMoved:
               ///< The joystick moved along an axis (data in event.joystickMove)
               return event.joystickMove;
          case sf::Event::EventType::JoystickConnected:
               ///< A joystick was connected (data in event.joystickConnect)
          case sf::Event::EventType::JoystickDisconnected:
               ///< A joystick was disconnected (data in event.joystickConnect)
               return event.joystickConnect;
          case sf::Event::EventType::TouchBegan:
               ///< A touch event began (data in event.touch)
          case sf::Event::EventType::TouchMoved:
               ///< A touch moved (data in event.touch)
          case sf::Event::EventType::TouchEnded:
               ///< A touch event ended (data in event.touch)
               return event.touch;
          case sf::Event::EventType::SensorChanged:
               ///< A sensor value changed (data in event.sensor)
               return event.sensor;
     }
}
}