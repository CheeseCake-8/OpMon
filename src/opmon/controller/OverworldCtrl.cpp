#include "OverworldCtrl.hpp"

#include "../model/objects/Attacks.hpp"
#include "../model/objects/OpMon.hpp"
#include "../model/sysObjects/OpTeam.hpp"
#include "BattleCtrl.hpp"
#include "EventsCtrl.hpp"
#include "PlayerCtrl.hpp"

namespace OpMon {
    namespace Controller {

        OverworldCtrl::OverworldCtrl(Model::Player &player, Model::UiData *uidata)
          : data(uidata, &player)
          , view("Player's room", this->data)
          , player(player) {}

        GameStatus OverworldCtrl::checkEvent(sf::Event const &events) {
            auto &overworld = view;

            bool is_dialog_open = overworld.getDialog() && !overworld.getDialog()->isDialogOver();

            switch(events.type) {
            case sf::Event::KeyPressed:
                if(events.key.code == sf::Keyboard::Equal) {
                    debugMode = !debugMode;
                    if(!debugMode) {
                        debugCol = false;
                        overworld.setCameraLock(false);
                    }
                }
                if(events.key.code == sf::Keyboard::M) {
                    reboot = true;
                    return GameStatus::STOP;
                }
                if(debugMode) {
                    if(events.key.code == sf::Keyboard::F10) {
                        overworld.printlayer[0] = !overworld.printlayer[0];
                    }
                    if(events.key.code == sf::Keyboard::F11) {
                        overworld.printlayer[1] = !overworld.printlayer[1];
                    }
                    if(events.key.code == sf::Keyboard::F12) {
                        overworld.printlayer[2] = !overworld.printlayer[2];
                    }
                    if(events.key.code == sf::Keyboard::C) {
                        overworld.printCollisions = !overworld.printCollisions;
                    }
                    if(events.key.code == sf::Keyboard::N) {
                        debugCol = !debugCol;
                    }
                    /*if(events.key.code == sf::Keyboard::B) {
                        data.getPlayer().addOpToOpTeam(new Model::OpMon("", data.getUiDataPtr()->getOp(4), 5, {Model::Attacks::newAtk("Tackle"), Model::Attacks::newAtk("Mist"), nullptr, nullptr}, Model::Nature::QUIET));
                        Model::OpTeam *opTeam = new Model::OpTeam("Unknown trainer");
                        opTeam->addOpMon(new Model::OpMon("", data.getUiDataPtr()->getOp(1), 5, {Model::Attacks::newAtk("Tackle"), Model::Attacks::newAtk("Mist"), nullptr, nullptr}, Model::Nature::QUIET));
                        _next_gs = new BattleCtrl(data.getPlayer().getOpTeam(), opTeam, data.getUiDataPtr(), data.getPlayerPtr());
                        data.getPlayer().healOp();
                        return GameStatus::NEXT;
			}*/
                    if(events.key.code == sf::Keyboard::Numpad5) {
                        overworld.setCameraLock(!overworld.isCameraLocked());
                    }
                }
            default:
                break;
            }
            if(overworld.isCameraLocked()) {
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad2)) {
                    overworld.moveCamera(Model::Side::TO_DOWN);
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad4)) {
                    overworld.moveCamera(Model::Side::TO_LEFT);
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad8)) {
                    overworld.moveCamera(Model::Side::TO_UP);
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad6)) {
                    overworld.moveCamera(Model::Side::TO_RIGHT);
                }
            }

            if(is_dialog_open) {
                return checkEventsDialog(events, overworld);
            } else {
                return checkEventsNoDialog(events, player);
            }
        }

        GameStatus OverworldCtrl::checkEventsDialog(sf::Event const &events, View::Overworld &overworld) {
            switch(events.type) {
            case sf::Event::KeyPressed:
                if(events.key.code == data.getUiDataPtr()->getKeyTalk()) {
                    overworld.getDialog()->pass();
                }
                break;
            default:
                break;
            }
            return GameStatus::CONTINUE;
        }

        GameStatus OverworldCtrl::checkEventsNoDialog(sf::Event const &event, Model::Player &player) {
            eventsctrl.checkAction(event, player, view);
            Controller::PlayerCtrl::checkMove(player, event, view);

            if(view.getBattleDeclared() != nullptr) {
                if(view.getBattleDeclared()->isDefeated()) {
                    view.endBattle();
                } else {
                    _next_gs = new BattleCtrl(data.getPlayer().getOpTeam(), view.getBattleDeclared(), data.getUiDataPtr(), data.getPlayerPtr());
                    return GameStatus::NEXT;
                }
            }

            return GameStatus::CONTINUE;
        }

        GameStatus OverworldCtrl::update(sf::RenderTexture &frame) {
            bool is_dialog_open = view.getDialog() && !view.getDialog()->isDialogOver();
            if(!is_dialog_open) {
                eventsctrl.updateEvents(data.getMap(player.getMapId())->getEvents(), player, view);
            }

            return view(frame);
        }

        void OverworldCtrl::suspend() {
            data.getUiDataPtr()->getJukebox().pause();
        }

        void OverworldCtrl::resume() {
            data.getUiDataPtr()->getJukebox().play(data.getCurrentMap()->getBg());
        }

    } // namespace Controller
} // namespace OpMon
