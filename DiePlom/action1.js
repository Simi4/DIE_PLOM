var modal1 = document.getElementById('signUp');        //div
var modal2 = document.getElementById('signIn');
var modal3 = document.getElementById('adminSignIn');

var modals = [modal1, modal2, modal3]

var formSignUp = document.getElementById('formSignUp');
var formSignIn = document.getElementById('formSignIn');
var adminFormSignIn = document.getElementById('adminFormSignIn');

var forms = [formSignUp, formSignIn, adminFormSignIn]

var but1 = document.getElementById("submit1").addEventListener("click", function() {
    sendToAdmin(1);
});
var but2 = document.getElementById("submit2").addEventListener("click", function() {
    sendToAdmin(2);
});
var but3 = document.getElementById("submit3").addEventListener("click", function() {
    sendToAdmin(3);
});

function errForm (err, modal) {
    // formSignUp.reset();
    alert(err)
    modal.style.display = "block";
}

function sendToAdmin(i) {
    var uname = document.getElementById("uname"+i.toString()).value;
    var psw = document.getElementById("psw"+i.toString()).value;
    if ((psw!=="" && uname!=="") && ((i === 1 && psw === document.getElementById("psw-repeat"+i.toString()).value) || (i !== 1))) {  //только в первой форме два пароля
        alert("Вы успешно зарегестрированы!")
        modals[i-1].style.display = "none";
        forms[i-1].reset();
        //use data
    } else if (uname==="") {
        errForm("Введите логин", modals[i-1])
    } else if (psw==="") {
        errForm("Введите пароль", modals[i-1])
    } else if (i===1) {
        errForm("Пароли не совпадают! Введите пароль еще раз.", modals[i-1])
    }
}