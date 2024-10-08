all:
	make clean
	make motor jogo bot
	@ clear
	@ echo Tudo compilado!
motor:
	gcc Motor.c -o motor -lncurses
	@ clear
	@ echo motor compilado!
jogo:
	gcc JogoUI.c -o jogo -lncurses
	@ clear
	@ echo jogo compilado!

bot:
	gcc bot.c -o bot
	@ clear
	@ echo bot compilado!

clean:
	$(RM) motor jogo bot 
	$(RM) motorfifo
	$(RM) jogofifo*
	$(RM) labirintofifo*
	$(RM) jogoenviarjogos*
	@ clear
	@ echo limpeza efetuada!
