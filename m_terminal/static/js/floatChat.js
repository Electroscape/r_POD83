const element = $('.floating-chat');

setTimeout(function () {
    element.addClass('enter');
    openElement();
}, 1000);

element.click(openElement);

function openElement() {
    const messages = element.find('.messages');
    const textInput = element.find('.text-box');
    element.find('#f-chat-icon').hide();
    element.addClass('expand');
    element.find('.chat').addClass('enter');
    textInput.keydown(onMetaAndEnter).focus();
    element.off('click', openElement);
    element.find('.header button').click(closeElement);
    element.find('#sendMessage').click(sendNewMessage);
    messages.scrollTop(messages.prop("scrollHeight"));
}

function closeElement() {
    element.find('.chat').removeClass('enter').hide();
    element.find('#f-chat-icon').show();
    element.removeClass('expand');
    element.find('.header button').off('click', closeElement);
    element.find('#sendMessage').off('click', sendNewMessage);
    element.find('.text-box').off('keydown', onMetaAndEnter).blur();
    setTimeout(function () {
        element.find('.chat').removeClass('enter').show()
        element.click(openElement);
    }, 500);
}


function sendNewMessage() {
    const userInput = $('.text-box');
    const newMessage = userInput.text();

    if (!newMessage) return;

    const messagesContainer = $('.messages');

    messagesContainer.prepend([
        '<li class="other">',
        `<b style="color: #000"> ${myRoomID}: </b>`,
        newMessage,
        '</li>'
    ].join(''));

    // clean out old message
    userInput.html('');
    // focus on input
    userInput.focus();

    messagesContainer.finish().animate({
        scrollTop: messagesContainer.prop("scrollHeight")
    }, 250)

    socket.emit('msg_to_backend', {
        user_name: myRoomID,
        message: newMessage
    })
}

function receiveNewMessage(src, msg) {
    const messagesContainer = $('.messages');

    messagesContainer.prepend([
        '<li class="other">',
        `<b style="color: #000"> ${src}: </b>`,
        msg,
        '</li>'
    ].join(''));

}

function onMetaAndEnter(event) {
    if (event.keyCode === 13) {
        sendNewMessage();
        return false;
    }
}