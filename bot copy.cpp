
#include <iostream>
#include <vector>
#include <dpp/dpp.h>

int main()
{
    dpp::cluster bot("your token here");

    bot.on_log(dpp::utility::cout_logger());

    bot.on_slashcommand([&bot](const dpp::slashcommand_t& event)
    {
        if (event.command.get_command_name() == "ping")
        {
            event.reply("Pong!");
        }

        if (event.command.get_command_name() == "clear")
        {
            bot.messages_get(event.command.channel_id, 0, 0, 0, 10,
                [&bot, event](const dpp::confirmation_callback_t& callback)
                {
                    if (callback.is_error())
                    {
                        event.reply("Error!");
                        return;
                    }

                    auto messages = callback.get<dpp::message_map>();

                    std::vector<dpp::snowflake> ids;

                    for (auto& message : messages)
                        ids.push_back(message.first);

                    if (ids.empty())
                    {
                        event.reply("no messages to delete!");
                        return;
                    }

                    bot.message_delete_bulk(ids, event.command.channel_id,
                        [event, count = ids.size()]
                        (const dpp::confirmation_callback_t& callback)
                        {
                            if (callback.is_error())
                            {
                                event.reply("cant delete!");
                                return;
                            }

                            event.reply(
                                "🗑️ Deleted " +
                                std::to_string(count) +
                                " messages!"
                            );
                        });
                }
            );
        }
    });

    bot.on_ready([&bot](const dpp::ready_t& event)
    {
        if (dpp::run_once<struct register_commands>())
        {
            bot.global_command_create(
                dpp::slashcommand("ping", "Ping pong!", bot.me.id)
            );

            bot.global_command_create(
                dpp::slashcommand(
                    "clear",
                    "Delete the last 10 messages",
                    bot.me.id
                )
            );
        }
    });

    bot.start(dpp::st_wait);
}

