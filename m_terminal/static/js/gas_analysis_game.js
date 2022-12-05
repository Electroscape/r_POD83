let correct = [];
let total = [];
let itemsList = []

function initiateGame(index, numItems) {
    let draggableItems = document.querySelector("#draggable-items-" + index);
    let matchingPairs = document.querySelector("#matching-pairs-" + index);

    const randomDraggableBrands = generateRandomItemsArray(numItems, brands);
    const alphabeticallySortedRandomDroppableBrands = [...randomDraggableBrands].sort((a, b) => a.brandName.toLowerCase().localeCompare(b.brandName.toLowerCase()));
    itemsList[index] = [...randomDraggableBrands];

    // Create "draggable-items" and append to DOM
    for (let i = 0; i < randomDraggableBrands.length; i++) {
        draggableItems.insertAdjacentHTML("beforeend", `
      <i class="fab fa-${randomDraggableBrands[i].iconName} draggable drag-g${index}" draggable="true" style="color: ${randomDraggableBrands[i].color};" id="${randomDraggableBrands[i].iconName}+"></i>
    `);
    }

    // Create "matching-pairs" and append to DOM
    for (let i = 0; i < alphabeticallySortedRandomDroppableBrands.length; i++) {
        matchingPairs.insertAdjacentHTML("beforeend", `
      <div class="matching-pair">
        <span class="gameLabel">${alphabeticallySortedRandomDroppableBrands[i].brandName}</span>
        <span class="droppable drop-g${index}" data-brand="${alphabeticallySortedRandomDroppableBrands[i].iconName}"></span>
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
    event.dataTransfer.setData("text", event.target.id); // or "text/plain"
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
    const index = 0;
    const scoreSection = document.querySelector("#score-" + index);
    const correctSpan = scoreSection.querySelector(".correct");
    const totalSpan = scoreSection.querySelector(".total");
    //const playAgainBtn = scoreSection.querySelector("#play-again-btn");

    event.target.classList.remove("droppable-hover");
    const draggableElementBrand = event.dataTransfer.getData("text");
    const droppableElementBrand = event.target.getAttribute("data-brand");
    const isCorrectMatching = draggableElementBrand === droppableElementBrand;
    total[index]++;
    const draggableElement = document.getElementById(draggableElementBrand);
    event.target.classList.add("dropped");
    draggableElement.classList.add("dragged");
    draggableElement.setAttribute("draggable", "false");
    event.target.innerHTML = `<i class="fab fa-${draggableElementBrand}" style="color: ${draggableElement.style.color};"></i>`;

    if (isCorrectMatching) {
        correct[index]++;
    }
    scoreSection.style.opacity = 0;
    setTimeout(() => {
        correctSpan.textContent = correct[index];
        totalSpan.textContent = total[index];
        scoreSection.style.opacity = 1;
    }, 200);
    if (itemsList[index].length === total[index] && correct[index] === total[index]) { // Game Over!!

    }
}

function playAgainBtnClick() {

    correct = 0;
    total = 0;
    draggableItems.style.opacity = 0;
    matchingPairs.style.opacity = 0;
    setTimeout(() => {
        scoreSection.style.opacity = 0;
    }, 100);
    setTimeout(() => {
        playAgainBtn.style.display = "none";
        while (draggableItems.firstChild) draggableItems.removeChild(draggableItems.firstChild);
        while (matchingPairs.firstChild) matchingPairs.removeChild(matchingPairs.firstChild);
        initiateGame();
        correctSpan.textContent = correct;
        totalSpan.textContent = total;
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
