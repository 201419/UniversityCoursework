clear; 
close all;

% ���� geneticAlgo ���м���
% - - - - - - - - - - - - - - - - - - - - - -
% �������Ĳ�������       10
% ��Ⱥ��ģ                100
% ÿһ���б��ֲ������Ŀ   50 (��������0.5)
% �������                0.1 (1/10�ĸ��巢������)
% ����ݻ�����            10000 (10000��)
% ��СĿ��ֵ              1.0e-6 (������Ӧ�Ⱥ���ֵ < 0.000001����)
[best_fitness11, best_fitness12, best_fitness21, best_fitness22, best_fitness31, best_fitness32, ...
    elite11, elite12, elite21, elite22, elite31, elite32, last_generation] = geneticAlgo(10, 100, 50, 0.1, 500);


% �����10��
% disp(best_fitness(9990:10000,:));
% disp(elite(9990:10000,:))
% �����ǲ����ʵģ���ΪGA�������м������ѭ����

% �������Ǻ��ʵ����
disp('��������Ϊ�� ');
disp(last_generation);
disp('�����Ӧ�ȣ� ');
% disp(best_fitness(last_generation-9:last_generation,:));
best_fitness11(last_generation,:)
disp('��Ѹ��壺 ');
% disp(elite(last_generation-9:last_generation,:));
elite11(last_generation,:)

% �����Ӧ�ȵ��ݻ����
% figure
% loglog(1:last_generation, best_fitness11(1:last_generation), 'linewidth',2)
% loglog(1:last_generation, best_fitness12(1:last_generation), 'linewidth',2)
% loglog(1:last_generation, best_fitness21(1:last_generation), 'linewidth',2)
% loglog(1:last_generation, best_fitness22(1:last_generation), 'linewidth',2)
% loglog(1:last_generation, best_fitness31(1:last_generation), 'linewidth',2)
% loglog(1:last_generation, best_fitness32(1:last_generation), 'linewidth',2)
% xlabel('Generation','fontsize',15);
% ylabel('Best Fitness','fontsize',15);
% set(gca,'fontsize',15,'ticklength',get(gca,'ticklength')*2);

% figure
% loglog(1:last_generation, best_fitness11(1:last_generation), 1:last_generation, best_fitness12(1:last_generation), ...
%     1:last_generation, best_fitness21(1:last_generation), 1:last_generation, best_fitness22(1:last_generation), ...
%     1:last_generation, best_fitness31(1:last_generation), 1:last_generation, best_fitness32(1:last_generation));

hold on;
plot(best_fitness11,'DisplayName','cross 1 and mutation 1');
plot(best_fitness12,'DisplayName','cross 1 and mutation 2');
plot(best_fitness21,'DisplayName','cross 2 and mutation 1');
plot(best_fitness22,'DisplayName','cross 2 and mutation 2');
plot(best_fitness31,'DisplayName','cross 3 and mutation 1');
plot(best_fitness32,'DisplayName','cross 3 and mutation 2');
hold off;

% ���Ž���ݻ����
% figure
% semilogx(1:last_generation, elite11(1:last_generation,:))
% xlabel('Generation','fontsize',15);
% ylabel('Best Solution','fontsize',15);
% set(gca,'fontsize',15,'ticklength',get(gca,'ticklength')*2);