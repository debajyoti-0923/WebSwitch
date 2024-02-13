let status=[0,2,0,2,1];

function update_status(){
    for(var i=0;i<5;i++){
        var btn=document.getElementById('btn'+(i+1));
        var timer=document.getElementById('timer'+(i+1));

        if(status[i]===0){
            btn.classList.add('btn');
            btn.classList.remove('btn-active');
        }else{
            btn.classList.add('btn-active');
            btn.classList.remove('btn');
        }

        if(status[i]==2){
            timer.classList.add('timer-active');
            timer.classList.remove('timer');
        }else{
            timer.classList.add('timer');
            timer.classList.remove('timer-active');
        }

    }
}

let btn1=document.getElementById('btn1');
let btn2=document.getElementById('btn2');
let btn3=document.getElementById('btn3');
let btn4=document.getElementById('btn4');
let btn5=document.getElementById('btn5');
let btn6=document.getElementById('btn6');

let timer1=document.getElementById('timer1');
let timer2=document.getElementById('timer2');
let timer3=document.getElementById('timer3');
let timer4=document.getElementById('timer4');
let timer5=document.getElementById('timer5');
let timer6=document.getElementById('timer6');

timer1.addEventListener('click',()=>{
    status[0]=2;
    update_status();
    alert('remaining time');
})

btn1.addEventListener('click',()=>{
    status[0]=0;
    update_status();
})



update_status()