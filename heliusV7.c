#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include <math.h>


int i2c_fd;
void ctrl_c(int sig)
{
        close(i2c_fd);
        exit(-1);
}

int main(void)
{
        char user_input=1, slave_addr=0x0F;
        char msp430_ret;
        unsigned int c = 0;
        double a1,a2,a3,a4,a5,a0,rot;
        double resoluc = (3.53)/1023;
        double kbat= 8.19/(8.19 + 9.36);
        double kger = 3.85/(3.85 + 9.9);
	double ldr_100=0, ldr_500=0, ldr_1k=0, G=0;
	double krad = (5/3.5);

	double uv_sensor, uv_irad, uv_index;

	double ad5,ad4,ad3,ad2,ad1,ad0;
        double media_temp, divt1, divt2, corgerad, corbat, pot_bat, pot_ger;	

	int i_date=0;
	
	while(1){

        //Configurando comunicação i2c
	signal(SIGINT, ctrl_c);
        i2c_fd = open("/dev/i2c-1", O_RDWR);
        ioctl(i2c_fd, I2C_SLAVE, slave_addr);

        int i=0;
        
	//Comunicação I2C com a MSPA	
        for(int cont=1;cont<=7;cont++)
        {
                user_input = cont;
                
                write(i2c_fd, &user_input, 1);
                sleep(0.5);
                read(i2c_fd, &msp430_ret,1);
                sleep(0.5);

                if (cont==1){
                        a5=msp430_ret*(1023/255);
                        printf("Valor do sensor em A5 (SEM NADA) é: %.2f\n", a5);
                }
                else if (cont==2){
                        a4=msp430_ret*(1023/255);
                        ldr_100=a4*resoluc;
                        printf("Valor do sensor em A4 (LDR) é: %.2f\n", a4);
			printf("Valor ldr_100 : %.2f\n", ldr_100);
                }
                else if (cont==3){
                        a3=msp430_ret*(1023/255);
			uv_sensor = a3*resoluc;
                        printf("Valor do sensor em A3 (UV) é: %.2f\n", a3);
			printf("Valor do UV_SENSOR = %.2f\n",uv_sensor);
                }
                else if (cont==4){
                        a2=((1023/255)*msp430_ret*100*resoluc)-273.15;
                        printf("Valor do sensor em A2 (LM335_1) é %.2f\n", a2);
                }
                else if (cont==5){
                        a1=((1023/255)*msp430_ret*100*resoluc)-273.15;
                        printf("Valor do sensor em A1 (LM335_2) é %.2f\n", a1);
                }
                else if (cont==6){
                        a0=(msp430_ret*(1023/255)*100*resoluc)-273.15;
	                printf("Valor do sensor em A0 (LM335_3_PISTÃO) é %.2f\n", a0);
                }
		else if (cont==7){
			rot=(msp430_ret*(1023/255)*100*resoluc);
			printf("Rotação do Stirling = %.2f\n", rot);
		}
                
                puts("");
		
        }
        
        close(i2c_fd);
	char user_input2=1, msp430_ret2, slave_addr2=0x0A;

        signal(SIGINT, ctrl_c);
        i2c_fd = open("/dev/i2c-1", O_RDWR);
        ioctl(i2c_fd, I2C_SLAVE, slave_addr2);
	
	//Configuração I2C com a MSPB
        for(int cont2=1;cont2<=6;cont2++)
        {
                user_input2 = cont2;
		write(i2c_fd, &user_input2, 1);
                sleep(0.5);
                read(i2c_fd, &msp430_ret2,1);
                sleep(0.5);
                
                if (cont2==1){
                        ad5=msp430_ret2*(1023/255);
			ldr_500=ad5*resoluc;
                        printf("Valor do sensor em AD5 (LDR_2) é: %.2f\n", ad5);
			printf("Valor do ldr_500: %.2f\n", ldr_500);
                }
                else if (cont2==2){
                        ad4=msp430_ret2*(1023/255);
			ldr_1k=ad4*resoluc;
                        printf("Valor do sensor em AD4 (LDR_3) é: %.2f\n", ad4);
			printf("Valor do ldr_1k: %.2f\n", ldr_1k);
                }
                else if (cont2==3){
                        ad3=msp430_ret2*(1023/255);
                        corbat = ((((ad3*resoluc)-2.5)*1000)/185);
                        printf("Valor do sensor em AD3 (CORRENTE_BATERIA): %.2f\n", ad3);
                        printf("Valor de CORRENTE_BATERIA: %.2f \n",corbat);
                }
                else if (cont2==4){
                        ad2=msp430_ret2*(1023/255);
			divt1=(ad2*resoluc)/kbat;
                        printf("Valor do sensor em AD2 (TENSÃO BATERIA) é %.2f\n", ad2);
			printf("TENSÃO NA BATERIA = %f \n", divt1);
                }
                else if (cont2==5){
                        ad1=msp430_ret2*(1023/255); 
                        corgerad=((((ad1*resoluc)-2.5)*1000)/185);
                        printf("Valor do sensor em AD1 (CORRENTE_GERADOR): %.2f\n",ad1);
			printf("Valor de CORRENTE_GERADOR: %.2f\n",corgerad);
                }
                else if (cont2==6){
                        ad0=msp430_ret2*(1023/255);
                        divt2 = (ad0*resoluc)/kger;
                        printf("Valor do sensor em AD0 (TENSÃO_GERADOR) é %.2f\n", ad0);
                        printf("TENSÃO NO GERADOR = %.2f \n", divt2);
                }

                puts("");
        }

        close(i2c_fd);
	
	//Calculo para irradiação a partir dos dados obtidos nos LDRs
	if(1.93<ldr_1k<3.33){
		G = (47.5*(ldr_1k*ldr_1k)) - (4.4*(100)*ldr_1k) + 1048;
	}
	else if(0.266<ldr_100<0.427){
		G = (5.5*(1000)*(ldr_100*ldr_100)) - (6.3*1000*ldr_100)+(2*1000);
	}
	else if(0.224<ldr_100<0.273){
		G  = (1.93*10000*ldr_100*ldr_100)-(1.7*10000*ldr_100)+(4000);
	}
	else if(0.203<ldr_100<0.231){
		G = (4.5*10000*ldr_100*ldr_100)-(3.3*10000*ldr_100)+(6.500);
	}
	else if(0.259<ldr_500<0.294){
		G = (4.8*10000*ldr_500*ldr_500)-(4.2*10000*ldr_500)+9999.7; 
	}
	else {
		G=0;
	}
	
	G=1000-G;
	printf("Irradiação estimada: %.2f\n", G);
	
	//Faixas do UV Index
	if(uv_sensor<0.05){
		uv_index=0;
		uv_irad=uv_index/40;
	}
	else if(0.226<uv_sensor<0.318){
		uv_index=1;
		uv_irad=uv_index/40;
	}
	else if(0.317<uv_sensor<0.408){
		uv_index=2;
		uv_irad=uv_index/40;
	}
	else if(0.407<uv_sensor<0.503){
		uv_index=3;
		uv_irad=uv_index/40;
	}
	else if(0.502<uv_sensor<0.606){
		uv_index=4;
		uv_irad=uv_index/40;
	}
	else if(0.605<uv_sensor<0.696){
		uv_index=5;
		uv_irad=uv_index/40;
	}
	else if(0.695<uv_sensor<0.795){
		uv_index=6;
		uv_irad=uv_index/40;
	}
	else if(0.794<uv_sensor<0.881){
		uv_index=7;
		uv_irad=uv_index/40;
	}
	else if(0.880<uv_sensor<0.976){
		uv_index=8;
		uv_irad=uv_index/40;
	}
	else if(0.975<uv_sensor<1.079){
		uv_index=9;
		uv_irad=uv_index/40;
	}
	else if(1.078<uv_sensor<1.170){
		uv_index=10;
		uv_irad=uv_index/40;
	}
	else if(1.169<uv_sensor<1.260){
		uv_index=11;
		uv_irad=uv_index/40;
	}
	else if(1.259<uv_sensor<1.350){
		uv_index=12;
		uv_irad=uv_index/40;
	}

	printf("UV_index: %.2f Irradiacao UV: %.2f\n",uv_index,uv_irad);
	pot_bat=corbat*divt1;
	pot_ger=corgerad*divt2;
	printf("Potencia Gerador: %.2f Potencia Bateria %.2f\n",pot_ger,pot_bat);
	
	media_temp =(a1+a2)/2;
	FILE * fp; //int x[5] = {0,1,2,5,5};
	fp = fopen ("file.csv", "w+");
        
	//Escrevendo os dados no arquivo file.csv
        fprintf(fp, "TEMP_REFLETOR,TEMP_PFRIO,IRRAD_LDR,IRRAD_UV,VBAT,VGER,IBAT,IGER,ROT,TEMP_PQUENTE,GPS_LAT,GPS_LONG\n");
        fprintf(fp, "%f,%f,%f,%f,%f,%f,%f,%f,%f,",media_temp,a0,G,uv_irad,divt1,divt2,corbat,corgerad,rot);
        fclose(fp);

	char command2[50];
        strcpy(command2, "python termoparV1.py");
        system(command2);

        char command[50];

        strcpy(command, "python3 read_gps.py");
        system(command);

	char comanddate[50];
	char aspas = '"';
	
	if(i_date==0){	
		system("sudo python3 horario.py");
		i_date = 1;
	}

	char comand3[50];
	strcpy(comand3, "python3 script.py");
	system(comand3);

	sleep(5);

	}
        return (0);
}

