function [best_fitness11, best_fitness12, best_fitness21, best_fitness22, best_fitness31, best_fitness32, ...
    elite11, elite12, elite21, elite22, elite31, elite32, iGen] = geneticAlgo( ...
    varNum, ...         % �������Ĳ�������
    popuSize, ...       % ��Ⱥ��ģ��ÿһ��������Ŀ��
    parentsNum, ...     % ÿһ���б��ֲ������Ŀ�����˱��죩
    mutationsRate, ...  % �������
    genMax ...         % ����ݻ�����
)

% minimal_cost ...    % ��СĿ��ֵ������ֵԽС������Ӧ��Խ�ߣ�

    % ���ۼӸ��ʡ�
    % ���� parentsNum = 10
    % ���� parentsNum:-1:1 ��������һ������
    % ��ĸ sum(parentsNum:-1:1) ��һ����ͽ����һ������
    %
    % ���� 10   9   8   7   6   5   4   3   2   1
    % ��ĸ 55
    % ��� 0.1818  0.1636  0.1455  0.1273  0.1091  0.0909  0.0727  0.0545  0.0364  0.0182
    % �ۼ� 0.1818  0.3455  0.4909  0.6182  0.7273  0.8182  0.8909  0.9455  0.9818  1.0000
    %
    % ���������Կ������ۼӸ��ʺ�����һ����0��1������Խ��Խ���ĺ���
    % ��Ϊ����ӵĸ���Խ��ԽС�������ǽ������еģ�
    cumulativeProb = cumsum((parentsNum:-1:1) / sum(parentsNum:-1:1)); % ����Ϊ parentsNum ������

    % �����Ӧ��---ÿһ���������Ӧ�ȶ��ȳ�ʼ��Ϊ1
    best_fitness11 = ones(genMax, 1); best_fitness12 = ones(genMax, 1); 
    best_fitness21 = ones(genMax, 1); best_fitness22 = ones(genMax, 1); 
    best_fitness31 = ones(genMax, 1); best_fitness32 = ones(genMax, 1); 
    % ��Ӣ---ÿһ���ľ�Ӣ�Ĳ���ֵ���ȳ�ʼ��Ϊ0
    elite11 = zeros(genMax, varNum); elite12 = zeros(genMax, varNum); 
    elite21 = zeros(genMax, varNum); elite22 = zeros(genMax, varNum); 
    elite31 = zeros(genMax, varNum); elite32 = zeros(genMax, varNum); 
    % ��Ů���� = ��Ⱥ���� - ��ĸ��������ĸ��ÿһ���в������ı�ĸ��壩
    child_number = popuSize - parentsNum; % ÿһ����Ů����Ŀ

    % ��ʼ����Ⱥ
    % popuSize ��Ӧ������У�ÿһ�б�ʾ1�����壬����=����������Ⱥ������
    % varNum ��Ӧ������У�����=����������������������Щ������ʾ��
    population = 40 * (rand(popuSize, varNum) - 0.5);  % ÿһ������Ⱥ����
    population11 = population; population12 = population; 
    population21 = population; population22 = population; 
    population31 = population; population32 = population; 
    
    for iGen = 1 : genMax  % �ݻ�ѭ����ʼ
        % feval�����ݴ��뵽һ������õĺ�������м���
        % �� population ������� fitEval ��������
        % index��¼�����ÿ��ֵԭ��������
        % cost = feval(fitEval, population); % �������и������Ӧ��
        % cost = fitEval(population);
        % [cost, index] = sort(cost); % ����Ӧ�Ⱥ���ֵ��С��������
        cost11 = fitEval(population11); [cost11, index11] = sort(cost11);
        cost12 = fitEval(population12); [cost12, index12] = sort(cost12);
        cost21 = fitEval(population21); [cost21, index21] = sort(cost21);
        cost22 = fitEval(population22); [cost22, index22] = sort(cost22);
        cost31 = fitEval(population31); [cost31, index31] = sort(cost31);
        cost32 = fitEval(population32); [cost32, index32] = sort(cost32);

        % index(1:parentsNum) 
        % ǰ parentsNum ��cost��С�ĸ�������Ⱥ population �е�����
        % ѡ���ⲿ��( parentsNum ��)������Ϊ��ĸ����ʵ parentsNum ��Ӧ�������
        % population = population(index(1:parentsNum), :); 
        population11 = population11(index11(1:parentsNum), :); 
        population12 = population12(index12(1:parentsNum), :); 
        population21 = population21(index21(1:parentsNum), :); 
        population22 = population22(index22(1:parentsNum), :); 
        population31 = population31(index31(1:parentsNum), :); 
        population32 = population32(index32(1:parentsNum), :); 
        % �ȱ���һ���ֽ��ŵĸ��壬���Կ��� population �����ǲ��ϱ仯��
        % population �����һ��Ϊ�����ľ�Ӣ����
        % elite(iGen, :) = population(1, :); % ��¼���������Ž⣨��Ӣ��
        elite11(iGen, :) = population11(1, :); elite12(iGen, :) = population12(1, :); 
        elite21(iGen, :) = population21(1, :); elite22(iGen, :) = population22(1, :);
        elite31(iGen, :) = population31(1, :); elite32(iGen, :) = population32(1, :);

        % cost�ھ���ǰ���sort����󣬾����Ѿ��ı�Ϊ�����
        % best_fitness(iGen) = cost(1); % ��¼�����������Ӧ��
        best_fitness11(iGen) = cost11(1); best_fitness12(iGen) = cost12(1);
        best_fitness21(iGen) = cost21(1); best_fitness22(iGen) = cost22(1);
        best_fitness31(iGen) = cost31(1); best_fitness32(iGen) = cost32(1);
%         if best_fitness(iGen) < minimal_cost; % ���������Ž����㹻�ã���ֹͣ�ݻ�
%             break; 
%         end


        % Ⱦɫ�彻�濪ʼ
        for child = 1:2:child_number % ����Ϊ2����Ϊÿһ�ν�������2������
            
            % ���Բ�ͬ�ĸ���ѡȡ������
            % cumulativeProb ����Ϊ parentsNum
            % �������ѡ��2����ĸ����  (child+parentsNum)%parentsNum
            % ��ǰ parentsNum ��cost��С�ĸ��������ѡ��
            % ���� cumulativeProb ����rand�ĵ�һ�������±�
            mother = find(cumulativeProb > rand, 1);
            father = find(cumulativeProb > rand, 1);
            
            % ������Ľ�����ý���������ֵ�ķ�ʽ��
            % ceil���컨�壩����ȡ����rand ����һ�������
            % �����ѡ����һ�У���һ�е�ֵ����
            % ����crossoverPoint=3, varNum=5
            % mask1 = 1   1   1   0   0
            % mask2 = 0   0   0   1   1
            crossoverPoint = ceil(rand*varNum); % ���ȷ��һ��Ⱦɫ�彻���
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
            
            % ��ȡ�ֿ���4��Ⱦɫ�壬ע���� .*
%             mother_1 = mask1 .* population(mother, :); % ĸ��Ⱦɫ���ǰ����
%             mother_2 = mask2 .* population(mother, :); % ĸ��Ⱦɫ��ĺ󲿷�
%             father_1 = mask1 .* population(father, :); % ����Ⱦɫ���ǰ����
%             father_2 = mask2 .* population(father, :); % ����Ⱦɫ��ĺ󲿷�
            % �õ���һ������������
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
            
        end % Ⱦɫ�彻�����

        
        % Ⱦɫ����쿪ʼ

%         mutationPopulation = population(2:popuSize, :);  % ��Ӣ���������
%         geneNum = (popuSize - 1) * varNum;  % ȫ��������Ŀ
%         mutationsNum = ceil(geneNum * mutationsRate);  % ����Ļ�����Ŀ
        geneNum = (popuSize - 1) * varNum;
        mutationsNum = ceil(geneNum * mutationsRate);
        mutationPopulation11 = population11(2:popuSize, :); mutationPopulation12 = population12(2:popuSize, :);
        mutationPopulation21 = population21(2:popuSize, :); mutationPopulation22 = population22(2:popuSize, :);
        mutationPopulation31 = population31(2:popuSize, :); mutationPopulation32 = population32(2:popuSize, :);

        % rand(1,mutationsNum) ���� mutationsNum �������(��Χ0-1)��ɵľ���
        % ���˺󣬾���ÿ��Ԫ�ر�ʾ�����ı�Ļ����λ�ã�Ԫ���ھ����е�һά���꣩
        % ȷ��Ҫ����Ļ����λ��
        mutationPoints = ceil(geneNum * rand(1, mutationsNum)); 

        % ������ͨ�������ֵ��ʵ�֡�
        % ��ѡ�еĻ��򶼱�һ��������������ɱ���
        % ��ѡ�еĻ�����б������
        % ����˳��
        % 1 4 7 10 13
        % 2 5 8 11 14
        % 3 6 9 12 15
%         mutationPopulation(mutationPoints) = 40 * (rand(1,mutationsNum) - 0.5); 
%         population(2:popuSize, :) = mutationPopulation; %��������֮�����Ⱥ
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

        % Ⱦɫ��������

    end % �ݻ�ѭ������
    
end

function [y] = fitEval(population)
    % population�������[0,1]��������Ĳ����ı䷶ΧΪ[-1,1]
    % population = 2 * (population - 0.5); 
    y = sum(population.^2, 2); % �е�ƽ����
end
