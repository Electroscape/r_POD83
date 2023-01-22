let correct = [];
let total = [];
let itemsList = []

function initiateGame(index, randomDraggableBrands) {
    let draggableItems = document.querySelector("#draggable-items-" + index);
    let matchingPairs = document.querySelector("#matching-pairs-" + index);
    let shadow = document.querySelector("#shadow-" + index);
    const submitBtn = document.querySelector("#submit-btn-" + index);

    submitBtn.disabled = true;
    shadow.hidden = true

    itemsList[index] = randomDraggableBrands.concat([]);
    const alphabeticallySortedRandomDroppableBrands = [...randomDraggableBrands].sort((a, b) => a.brandName.toLowerCase().localeCompare(b.brandName.toLowerCase()));


    // Create "draggable-items" and append to DOM
    for (let i = 0; i < randomDraggableBrands.length; i++) {
        draggableItems.insertAdjacentHTML("beforeend", `
      <i class="fab fa-${randomDraggableBrands[i].iconName} draggable drag-g${index}" draggable="true" style="color: ${randomDraggableBrands[i].color};" id="${randomDraggableBrands[i].iconName}-${index}"></i>
    `);
    }

    // Create "matching-pairs" and append to DOM
    for (let i = 0; i < alphabeticallySortedRandomDroppableBrands.length; i++) {
        matchingPairs.insertAdjacentHTML("beforeend", `
      <div class="matching-pair">
        <span class="gameLabel">${alphabeticallySortedRandomDroppableBrands[i].brandName}</span>
        <span class="drop-g${index} droppable" data-brand="${alphabeticallySortedRandomDroppableBrands[i].iconName}-${index}"></span>
      </div>
    `);
    }

    let draggableElements = document.querySelectorAll(".drag-g" + index);
    let droppableElements = document.querySelectorAll(".drop-g" + index);

    draggableElements.forEach(elem => {
        elem.addEventListener("dragstart", dragStart);
        // elem.addEventListener("drag", drag);
        // elem.addEventListener("dragend", dragEnd);
    });

    droppableElements.forEach(elem => {
        elem.addEventListener("dragenter", dragEnter);
        elem.addEventListener("dragover", dragOver);
        elem.addEventListener("dragleave", dragLeave);
        elem.addEventListener("drop", drop);
    });
}

// Drag and Drop Functions

//Events fired on the drag target

function dragStart(event) {
    let itemId = event.target.id;
    event.dataTransfer.setData("text", itemId); // or "text/plain"
}

//Events fired on the drop target

function dragEnter(event) {
    if (event.target.classList && event.target.classList.contains("droppable") && !event.target.classList.contains("dropped")) {
        event.target.classList.add("droppable-hover");
    }
}

function dragOver(event) {
    if (event.target.classList && event.target.classList.contains("droppable") && !event.target.classList.contains("dropped")) {
        event.preventDefault();
    }
}

function dragLeave(event) {
    if (event.target.classList && event.target.classList.contains("droppable") && !event.target.classList.contains("dropped")) {
        event.target.classList.remove("droppable-hover");
    }
}

function drop(event) {
    event.preventDefault();
    const index = event.target.classList[0].at(-1);
    const scoreSection = document.querySelector("#score-" + index);
    const correctSpan = scoreSection.querySelector(".correct");
    const totalSpan = scoreSection.querySelector(".total");
    const submitBtn = document.querySelector("#submit-btn-" + index);

    event.target.classList.remove("droppable-hover");
    const draggableElementBrand = event.dataTransfer.getData("text");
    const droppableElementBrand = event.target.getAttribute("data-brand");
    const isCorrectMatching = draggableElementBrand === droppableElementBrand;
    total[index]++;

    const draggableElement = document.getElementById(draggableElementBrand);
    event.target.classList.add("dropped");
    draggableElement.classList.add("dragged");
    draggableElement.setAttribute("draggable", "false");
    let iconName = draggableElementBrand.split("-");
    iconName.pop()
    event.target.innerHTML = `<i class="fab fa-${iconName.join("-")}" style="color: ${draggableElement.style.color};"></i>`;

    if (isCorrectMatching) {
        correct[index]++;
    }
    scoreSection.style.opacity = 0;
    setTimeout(() => {
        correctSpan.textContent = correct[index];
        totalSpan.textContent = total[index];
        scoreSection.style.opacity = 1;
    }, 200);
    if (itemsList[index].length === total[index]) { // Game Over!!
        submitBtn.disabled = false;
    } else if (itemsList[index].length === total[index]) {
        setTimeout(() => {
            alert("Unstable combination")
            $("#reset-btn-" + index).click()
        }, 600)
    }
}

function submitBtnClick(index) {
    if (itemsList[index].length === total[index] && correct[index] === total[index]) { // Game Over!!
        socket.emit('msg_to_backend', {
            keypad_update: `${window.location.pathname} dragDropGame ${index} correct`
        })
        setTimeout(() => {
            alert("Stable Correct Solution");
        }, 2000)
    } else if (itemsList[index].length === total[index]) {
        socket.emit('msg_to_backend', {
            keypad_update: `${window.location.pathname} dragDropGame ${index} wrong`
        })
        setTimeout(() => {
            alert("Unstable combination")
            $("#reset-btn-" + index).click()
        }, 2000)
    }
}

function resetBtnClick(index) {
    let draggableItems = document.querySelector("#draggable-items-" + index);
    let matchingPairs = document.querySelector("#matching-pairs-" + index);

    const scoreSection = document.querySelector("#score-" + index);
    const correctSpan = scoreSection.querySelector(".correct");
    const totalSpan = scoreSection.querySelector(".total");
    const submitBtn = document.querySelector("#submit-btn-" + index);

    submitBtn.disabled = true;
    correct[index] = 0;
    total[index] = 0;
    draggableItems.style.opacity = 0;
    matchingPairs.style.opacity = 0;
    setTimeout(() => {
        scoreSection.style.opacity = 0;
    }, 100);
    setTimeout(() => {
        while (draggableItems.firstChild) draggableItems.removeChild(draggableItems.firstChild);
        while (matchingPairs.firstChild) matchingPairs.removeChild(matchingPairs.firstChild);
        initiateGame(index, itemsList[index]);
        correctSpan.textContent = correct[index];
        totalSpan.textContent = total[index];
        draggableItems.style.opacity = 1;
        matchingPairs.style.opacity = 1;
        scoreSection.style.opacity = 1;
    }, 500);
}

// Auxiliary functions
function generateRandomItemsArray(n, originalArray) {
    let res = [];
    let clonedArray = [...originalArray];
    if (n > clonedArray.length) n = clonedArray.length;
    for (let i = 1; i <= n; i++) {
        const randomIndex = Math.floor(Math.random() * clonedArray.length);
        res.push(clonedArray[randomIndex]);
        clonedArray.splice(randomIndex, 1);
    }
    return res;
}
