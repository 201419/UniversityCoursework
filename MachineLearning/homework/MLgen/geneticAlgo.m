function [best_fitness11, best_fitness12, best_fitness21, best_fitness22, best_fitness31, best_fitness32, ...
    elite11, elite12, elite21, elite22, elite31, elite32, iGen] = geneticAlgo( ...
    varNum, ...         % 求解问题的参数个数
    popuSize, ...       % 种群规模（每一代个体数目）
    parentsNum, ...     % 每一代中保持不变的数目（除了变异）
    mutationsRate, ...  % 变异概率
    genMax ...         % 最大演化代数
)

% minimal_cost ...    % 最小目标值（函数值越小，则适应度越高）

    % 【累加概率】
    % 假设 parentsNum = 10
    % 分子 parentsNum:-1:1 用于生成一个数列
    % 分母 sum(parentsNum:-1:1) 是一个求和结果（一个数）
    %
    % 分子 10   9   8   7   6   5   4   3   2   1
    % 分母 55
    % 相除 0.1818  0.1636  0.1455  0.1273  0.1091  0.0909  0.0727  0.0545  0.0364  0.0182
    % 累加 0.1818  0.3455  0.4909  0.6182  0.7273  0.8182  0.8909  0.9455  0.9818  1.0000
    %
    % 运算结果可以看出，累加概率函数是一个从0到1增长得越来越慢的函数
    % 因为后面加的概率越来越小（数列是降序排列的）
    cumulativeProb = cumsum((parentsNum:-1:1) / sum(parentsNum:-1:1)); % 长度为 parentsNum 的数列

    % 最佳适应度---每一代的最佳适应度都先初始化为1
    best_fitness11 = ones(genMax, 1); best_fitness12 = ones(genMax, 1); 
    best_fitness21 = ones(genMax, 1); best_fitness22 = ones(genMax, 1); 
    best_fitness31 = ones(genMax, 1); best_fitness32 = ones(genMax, 1); 
    % 精英---每一代的精英的参数值都先初始化为0
    elite11 = zeros(genMax, varNum); elite12 = zeros(genMax, varNum); 
    elite21 = zeros(genMax, varNum); elite22 = zeros(genMax, varNum); 
    elite31 = zeros(genMax, varNum); elite32 = zeros(genMax, varNum); 
    % 子女数量 = 种群数量 - 父母数量（父母即每一代中不发生改变的个体）
    child_number = popuSize - parentsNum; % 每一代子女的数目

    % 初始化种群
    % popuSize 对应矩阵的行，每一行表示1个个体，行数=个体数（种群数量）
    % varNum 对应矩阵的列，列数=参数个数（个体特征由这些参数表示）
    population = 40 * (rand(popuSize, varNum) - 0.5);  % 每一代的种群矩阵
    population11 = population; population12 = population; 
    population21 = population; population22 = population; 
    population31 = population; population32 = population; 
    
    for iGen = 1 : genMax  % 演化循环开始
        % feval把数据带入到一个定义好的函数句柄中计算
        % 把 population 矩阵带入 fitEval 函数计算
        % index记录排序后每个值原来的行数
        % cost = feval(fitEval, population); % 计算所有个体的适应度
        % cost = fitEval(population);
        % [cost, index] = sort(cost); % 将适应度函数值从小到大排序
        cost11 = fitEval(population11); [cost11, index11] = sort(cost11);
        cost12 = fitEval(population12); [cost12, index12] = sort(cost12);
        cost21 = fitEval(population21); [cost21, index21] = sort(cost21);
        cost22 = fitEval(population22); [cost22, index22] = sort(cost22);
        cost31 = fitEval(population31); [cost31, index31] = sort(cost31);
        cost32 = fitEval(population32); [cost32, index32] = sort(cost32);

        % index(1:parentsNum) 
        % 前 parentsNum 个cost较小的个体在种群 population 中的行数
        % 选出这部分( parentsNum 个)个体作为父母，其实 parentsNum 对应交叉概率
        % population = population(index(1:parentsNum), :); 
        population11 = population11(index11(1:parentsNum), :); 
        population12 = population12(index12(1:parentsNum), :); 
        population21 = population21(index21(1:parentsNum), :); 
        population22 = population22(index22(1:parentsNum), :); 
        population31 = population31(index31(1:parentsNum), :); 
        population32 = population32(index32(1:parentsNum), :); 
        % 先保留一部分较优的个体，可以看出 population 矩阵是不断变化的
        % population 矩阵第一行为本代的精英个体
        % elite(iGen, :) = population(1, :); % 记录本代的最优解（精英）
        elite11(iGen, :) = population11(1, :); elite12(iGen, :) = population12(1, :); 
        elite21(iGen, :) = population21(1, :); elite22(iGen, :) = population22(1, :);
        elite31(iGen, :) = population31(1, :); elite32(iGen, :) = population32(1, :);

        % cost在经过前面的sort排序后，矩阵已经改变为升序的
        % best_fitness(iGen) = cost(1); % 记录本代的最佳适应度
        best_fitness11(iGen) = cost11(1); best_fitness12(iGen) = cost12(1);
        best_fitness21(iGen) = cost21(1); best_fitness22(iGen) = cost22(1);
        best_fitness31(iGen) = cost31(1); best_fitness32(iGen) = cost32(1);
%         if best_fitness(iGen) < minimal_cost; % 本代的最优解已足够好，则停止演化
%             break; 
%         end


        % 染色体交叉开始
        for child = 1:2:child_number % 步长为2是因为每一次交叉会产生2个孩子
            
            % 【以不同的概率选取父代】
            % cumulativeProb 长度为 parentsNum
            % 从中随机选择2个父母出来  (child+parentsNum)%parentsNum
            % 从前 parentsNum 个cost较小的个体中随机选择
            % 返回 cumulativeProb 大于rand的第一个数的下标
            mother = find(cumulativeProb > rand, 1);
            father = find(cumulativeProb > rand, 1);
            
            % 【具体的交叉采用交换变量数值的方式】
            % ceil（天花板）向上取整，rand 生成一个随机数
            % 即随机选择了一列，这一列的值交换
            % 假如crossoverPoint=3, varNum=5
            % mask1 = 1   1   1   0   0
            % mask2 = 0   0   0   1   1
            crossoverPoint = ceil(rand*varNum); % 随机确定一个染色体交叉点
            mask1_1 = [ones(1,crossoverPoint), zeros(1,varNum-crossoverPoint)]; mask1_2 = not(mask1_1);
            mask2_1 = ones(1,varNum);
            for imask2 = 3:(varNum-3) 
                mask2_1(imask2) = 1;
            end
            mask2_2 = not(mask2_1);
            mask3_1 = zeros(1,varNum);
            for imask3 = 1:2:varNum 
                mask3_1(imask3) = 1;
            end
            mask3_2 = not(mask3_1);
            
            % 获取分开的4段染色体，注意是 .*
%             mother_1 = mask1 .* population(mother, :); % 母亲染色体的前部分
%             mother_2 = mask2 .* population(mother, :); % 母亲染色体的后部分
%             father_1 = mask1 .* population(father, :); % 父亲染色体的前部分
%             father_2 = mask2 .* population(father, :); % 父亲染色体的后部分
            % 得到下一代的两个孩子
%             population(parentsNum + child    , :) = mother_1 + father_2;
%             population(parentsNum + child + 1, :) = father_1 + mother_2;
            
            mother11_1 = mask1_1 .* population11(mother, :); mother11_2 = mask1_2 .* population11(mother, :);
            father11_1 = mask1_1 .* population11(father, :); father11_2 = mask1_2 .* population11(father, :);
            population11(parentsNum + child    , :) = mother11_1 + father11_2; population11(parentsNum + child + 1, :) = father11_1 + mother11_2;
            
            mother12_1 = mask1_1 .* population12(mother, :); mother12_2 = mask1_2 .* population12(mother, :);
            father12_1 = mask1_1 .* population12(father, :); father12_2 = mask1_2 .* population12(father, :);
            population12(parentsNum + child    , :) = mother12_1 + father12_2; population12(parentsNum + child + 1, :) = father12_1 + mother12_2;
            
            mother21_1 = mask2_1 .* population21(mother, :); mother21_2 = mask2_2 .* population21(mother, :);
            father21_1 = mask2_1 .* population21(father, :); father21_2 = mask2_2 .* population21(father, :);
            population21(parentsNum + child    , :) = mother21_1 + father21_2; population21(parentsNum + child + 1, :) = father21_1 + mother21_2;
            
            mother22_1 = mask2_1 .* population22(mother, :); mother22_2 = mask2_2 .* population22(mother, :);
            father22_1 = mask2_1 .* population22(father, :); father22_2 = mask2_2 .* population22(father, :);
            population22(parentsNum + child    , :) = mother22_1 + father22_2; population22(parentsNum + child + 1, :) = father22_1 + mother22_2;
            
            mother31_1 = mask3_1 .* population31(mother, :); mother31_2 = mask3_2 .* population31(mother, :);
            father31_1 = mask3_1 .* population31(father, :); father31_2 = mask3_2 .* population31(father, :);
            population31(parentsNum + child    , :) = mother31_1 + father31_2; population31(parentsNum + child + 1, :) = father31_1 + mother31_2;
            
            mother32_1 = mask3_1 .* population32(mother, :); mother32_2 = mask3_2 .* population32(mother, :);
            father32_1 = mask3_1 .* population32(father, :); father32_2 = mask3_2 .* population32(father, :);
            population32(parentsNum + child    , :) = mother32_1 + father32_2; population32(parentsNum + child + 1, :) = father32_1 + mother32_2;
            
        end % 染色体交叉结束

        
        % 染色体变异开始

%         mutationPopulation = population(2:popuSize, :);  % 精英不参与变异
%         geneNum = (popuSize - 1) * varNum;  % 全部基因数目
%         mutationsNum = ceil(geneNum * mutationsRate);  % 变异的基因数目
        geneNum = (popuSize - 1) * varNum;
        mutationsNum = ceil(geneNum * mutationsRate);
        mutationPopulation11 = population11(2:popuSize, :); mutationPopulation12 = population12(2:popuSize, :);
        mutationPopulation21 = population21(2:popuSize, :); mutationPopulation22 = population22(2:popuSize, :);
        mutationPopulation31 = population31(2:popuSize, :); mutationPopulation32 = population32(2:popuSize, :);

        % rand(1,mutationsNum) 生成 mutationsNum 个随机数(范围0-1)组成的矩阵
        % 数乘后，矩阵每个元素表示发生改变的基因的位置（元素在矩阵中的一维坐标）
        % 确定要变异的基因的位置
        mutationPoints = ceil(geneNum * rand(1, mutationsNum)); 

        % 【变异通过随机赋值来实现】
        % 被选中的基因都被一个随机数替代，完成变异
        % 对选中的基因进行变异操作
        % 计数顺序
        % 1 4 7 10 13
        % 2 5 8 11 14
        % 3 6 9 12 15
%         mutationPopulation(mutationPoints) = 40 * (rand(1,mutationsNum) - 0.5); 
%         population(2:popuSize, :) = mutationPopulation; %发生变异之后的种群
        mutationPopulation11(mutationPoints) = 40 * (rand(1,mutationsNum) - 0.5); 
        population11(2:popuSize, :) = mutationPopulation11;
        mutationPopulation21(mutationPoints) = 40 * (rand(1,mutationsNum) - 0.5); 
        population21(2:popuSize, :) = mutationPopulation21;
        mutationPopulation31(mutationPoints) = 40 * (rand(1,mutationsNum) - 0.5); 
        population31(2:popuSize, :) = mutationPopulation31;
        
        mutationPopulation12(mutationPoints) = mutationPopulation12(mutationPoints) + 2 * (rand(1,mutationsNum) - 0.5); 
        population12(2:popuSize, :) = mutationPopulation12;
        mutationPopulation22(mutationPoints) = mutationPopulation22(mutationPoints) + 2 * (rand(1,mutationsNum) - 0.5); 
        population22(2:popuSize, :) = mutationPopulation22;
        mutationPopulation32(mutationPoints) = mutationPopulation32(mutationPoints) + 2 * (rand(1,mutationsNum) - 0.5); 
        population32(2:popuSize, :) = mutationPopulation32;

        % 染色体变异结束

    end % 演化循环结束
    
end

function [y] = fitEval(population)
    % population是随机数[0,1]矩阵，下面的操作改变范围为[-1,1]
    % population = 2 * (population - 0.5); 
    y = sum(population.^2, 2); % 行的平方和
end
