#include "SFMLRunner.h"



void SFMLRunner::run()
{
    sf::Text text;
    text.setPosition(90, 500);
    text.setCharacterSize(30);
 
    sf::Clock clock;
    sf::String textSuccess = "Message was sent", textError = "Input text to send it, please";
    sf::RenderWindow window(sf::VideoMode(800, 800), "SFML works!");
    sf::RectangleShape fieldText(sf::Vector2f(600, 520));
    fieldText.setFillColor(sf::Color::Black);
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("background.jpg")) {
        throw 1; //todo
    }
    sf::Sprite sBackground(backgroundTexture);

    sf::Texture button;
    sf::Sprite sButton;
    sf::Clock myClock;
    bool showCursor = false;
    
    std::wstring message = L"";

    sf::Font font;
    if (!font.loadFromFile("Menlo-Regular.ttf")) {
        throw 3;
    }
    text.setPosition(250, 650);
    text.setCharacterSize(30);
    int messageLength = 0;
    sf::Text messageText;
    text.setFont(font);
    messageText.setFont(font);
    messageText.setStyle(sf::Text::Bold);
    messageText.setString(message);
    messageText.setFillColor(sf::Color::White);
    messageText.setCharacterSize(30);
    messageText.setPosition(115, 70);
    int widthLowLetter = 26, widthUpperLetter = 26;
    wchar_t prevSymb = L'0';
    std::vector<int> lineLength;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::TextEntered) {
                wchar_t symb = event.text.unicode;
                if (symb == '\b') {
                    if (!message.empty()){
                        prevSymb = message.at(message.size() - 1);
                        message.pop_back();
                        if (prevSymb >= 'A' && prevSymb <= 'Z' || prevSymb >= 1040 && prevSymb <= 1071) {
                            messageLength -= widthUpperLetter;
                        }
                        else if (prevSymb != '\n') {
                            messageLength -= widthLowLetter;
                        }
                        else {
                            messageLength = lineLength.back();
                            lineLength.pop_back();
                        }
                    }
                }
                else if (symb == '\r' && lineLength.size() < 11) {
                    
                    lineLength.push_back(messageLength);
                    messageLength = 0;
                    message += '\n';
                    

                    //messageText.getGlobalBounds();
                }
                else if(lineLength.size() < 11){
                    if (messageLength >= 770 && lineLength.size() < 10) {
                        lineLength.push_back(messageLength);
                        messageLength = 0;
                        message += '\n';
                    }
                    else if (messageLength >= 770 && lineLength.size() >= 10) {
                        break;
                    }
                    message += symb;
                    if (symb >= 'A' && symb <= 'Z' || symb >= 1040 && symb <= 1071) {
                        messageLength += widthUpperLetter;
                    }
                    else{
                        messageLength += widthLowLetter;
                    }
                }
                messageText.setString(message);
            }

            if (event.type == sf::Event::MouseMoved) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
                if (sButton.getGlobalBounds().contains(mousePosF)) {
                    bool showCursor = false;
                    if (!button.loadFromFile("buttons.png")) {
                        throw 2; //todo
                    }
                    sButton.setPosition(90.0f, 600.0f);
                    sButton.setTextureRect(sf::IntRect(173, 0, 155, 155));
                    sButton.setTexture(button);
                    fieldText.setPosition(100.0f, 50.0f);
                }
                else {
                    bool showCursor = false;
                    if (!button.loadFromFile("buttons.png")) {
                        throw 2; //todo
                    }
                    sButton.setPosition(90.0f, 600.0f);
                    sButton.setTextureRect(sf::IntRect(0, 170, 155, 250));//koordinatbl nyzhnbl
                    sButton.setTexture(button);
                    fieldText.setPosition(100.0f, 50.0f);
                }
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
                if (sButton.getGlobalBounds().contains(mousePosF)) {
                    clock.restart();
                    if (message != L"") {
                        saveMessage(message);
                        message = L"";
                        lineLength.clear();

                        text.setString(textSuccess);
                        text.setFillColor(sf::Color::Green);
                    }
                    else{
                        text.setString(textError);
                        text.setFillColor(sf::Color::Red);
                        
                    }
                }
            }
        }

        window.clear();
        window.draw(sBackground);
        window.draw(sButton);
        window.draw(fieldText);
        window.draw(messageText);
        if (clock.getElapsedTime().asSeconds() < 2) {
            window.draw(text);
        }
        if (myClock.getElapsedTime() >= sf::milliseconds(500))
        {
            myClock.restart();
            showCursor = !showCursor;
            if (showCursor)
                messageText.setString(message + L"|");
            else
                messageText.setString(message);
        }

        window.display();

    }
}

void SFMLRunner::saveMessage(std::wstring message) {
    MySQLConnection *connector = MySQLConnection::getInstance();
    connector->addNewAdvert(message);

}
