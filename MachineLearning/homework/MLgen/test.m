clear; 
close all;

% 调用 geneticAlgo 进行计算
% - - - - - - - - - - - - - - - - - - - - - -
% 求解问题的参数个数       10
% 种群规模                100
% 每一代中保持不变的数目   50 (即交叉率0.5)
% 变异概率                0.1 (1/10的个体发生变异)
% 最大演化代数            10000 (10000代)
% 最小目标值              1.0e-6 (个体适应度函数值 < 0.000001结束)
[best_fitness11, best_fitness12, best_fitness21, best_fitness22, best_fitness31, best_fitness32, ...
    elite11, elite12, elite21, elite22, elite31, elite32, last_generation] = geneticAlgo(10, 100, 50, 0.1, 500);


% 输出后10行
% disp(best_fitness(9990:10000,:));
% disp(elite(9990:10000,:))
% 这样是不合适的，因为GA常常在中间就跳出循环了

% 这样才是合适的输出
disp('迭代次数为： ');
disp(last_generation);
disp('最佳适应度： ');
% disp(best_fitness(last_generation-9:last_generation,:));
best_fitness11(last_generation,:)
disp('最佳个体： ');
% disp(elite(last_generation-9:last_generation,:));
elite11(last_generation,:)

% 最佳适应度的演化情况
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

% 最优解的演化情况
% figure
% semilogx(1:last_generation, elite11(1:last_generation,:))
% xlabel('Generation','fontsize',15);
% ylabel('Best Solution','fontsize',15);
% set(gca,'fontsize',15,'ticklength',get(gca,'ticklength')*2);