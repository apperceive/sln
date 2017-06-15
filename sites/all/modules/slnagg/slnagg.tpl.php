<style>
.school { 
  color: red;
}
h3.sln-subheading, h4.sln-subheding  {
  color: #a00;
}
.interpretation {

}
.interpretation:hover {
  border: solid thin red;
  background-color: #eee;
}
.report {
  width: 8in;
  margin: 0 auto;
  padding: 0 0 80px 0;
}

table, table td {
  border: solid thin black;
  
}
table thead th, table thead td {
  border: solid thin black;
  text-align: center;
  color: #ff0;
  background-color: #000;
}

td.left {
  text-align: left;
}
td.right {
  text-align: right;
}

table tr.even, table tr.odd {
  background-color: transparent;
}
</style>

<script src='/sites/all/libraries/jeditable/jquery.jeditable.mini.js'></script>
<script>
  jQuery(document).ready(function() {
     jQuery('.interpretation').editable('/au2017/annosave', {
         cancel    : 'Cancel',
         submit    : 'Save',
         type      : 'textarea',
         indicator : '<img src="/sites/all/libraries/jeditable/indicator.gif">',
         tooltip   : 'Click to edit...',
         submitdata : {school: "%SCHOOL%"}
     });
  });
</script>

<div class='report'>

%RETURN_LINK%

<h2 class='school sln-subheading'>%SCHOOL%</h2>


<p>The purpose of this SLN Report is to present the results of your school’s participation in PaSL program.  </p>


<p id='interpretation-0' class='interpretation'>%INTERPRETATION_0%</p>

</p>Members of your school provided information via the Strategic Thinking Questionnaire (STQ), the Strategic Leadership Questionnaire (SLQ), The Entrepreneurial Dispositions Scale (EDS) and the Entrepreneurial Orientation (EO) assessment.  Your results are found in the following paragraphs</p>

<h3 class='sln-subheading'>The Capacity of the leadership Team at %SCHOOL% to Think Strategically</h3>

<p>The STQ measures use of three thinking skills: Reframing, Reflection, and Systems Thinking (Pisapia, Reyes-Guerra, & Coukos-Semmel, 2005; Pisapia, Morris, Toussaint, & Ellington, 2011). </p>

<h4>Table 1</h4>
<p>The Capacity of the Leadership Team at %SCHOOL% to Think Strategically</p>

%TABLE1%

<p id='interpretation-1' class='interpretation'>%INTERPRETATION_1%</p>

<h3 class='sln-subheading'>The Capacity of the leadership Team at %SCHOOL%’s to use strategic leadership influence actions</h3>

<p>The SLQ measures five leader influence actions: Transforming, Managing, Bonding, Bridging, and Bartering (Pisapia, 2009; Reyes-Guerra, 2008; Yasin, 2006).These actions drive the SLM which  establishes a generative protocol that organizations and teams can follow to create a Statement of Intent to guide the actions (Pisapia, 2009; Pisapia & Robinson, 2011). </p>

<h4>Table 2</h4>
<p>The Capacity of %SCHOOL%'s leadership team to act strategically rated by the leadership team and teachers/observers</p>

%TABLE2%

<p id='interpretation-2' class='interpretation'>%INTERPRETATION_2%</p>

<h3 class='sln-subheading'>The Dispositions of the leadership team at %SCHOOL% to act entrepreneurially.</h3>

<p>The EOS assesses an individuals capacity for entrepreneurial leadership.</p>    

<h4>Table 3</h4>
<p>The Dispositions of the Leadership Team at %SCHOOL% rated by members of the Leadership Team</p>

%TABLE3%		

<p id='interpretation-3' class='interpretation'>%INTERPRETATION_3%</p>

<h3 class='sln-subheading'>The Entrepreneurial Culture at %SCHOOL%.</h3>

<p>The EO measures your schools entrepreneurial culture.  It answers the question that “does your school possess a entrepreneurial culture?”  The aggregate score is composed of measures of risk taking, innovativeness, and proactiveness.</p>

<h4>Table 4</h4>
<p>The Entrepreneurial Culture of %SCHOOL% rated by the Leadership Team and Teachers </p>

%TABLE4%  

<p id='interpretation-4' class='interpretation'>%INTERPRETATION_4%</p>



<h2 class='sln-subheading sln-newpage'>SLQ<sup>360</sup> Profile Report</h2>

<h3 class='sln-subheading'>The Leadership Challenge for Today’s Leaders</h3>

<p>Today’s environments are characterized by complexity, ambiguity and sometimes chaos and disruptive change. In these complex environments research shows that command and control actions are not as effective as they were when environments were more certain. Rather than surrendering to the environment, today’s leaders need a multifaceted set of leader actions to master it, drive performance and gain commitment.</p>

<h3 class='sln-subheading'>The SLQ<sup>360</sup></h3>

<p>The SLQ<sup>360</sup> was developed to measure the ability of executives, managers, and supervisors to use five research validated leader influence actions – <span class='important'>TRANSFORMING, MANAGING, BONDING, BRIDGING, and BARTERING</span> from your perspective, your coworker's perspective and your supervisor's perspective, if desired.</p>

<p>The SLQ<sup>360</sup> explores the manager’s use of the five leader influence actions that lead to organizational and individual effectiveness. The results conveyed are valid and relevant for career and performance success to the extent that participants answered the questions honestly, forthrightly, and to the best of your ability.</p>

<p>When a manager uses these influence actions in combinations, our research studies as well as the studies of others indicate that they and their organizations will become more effective.  The SLQ<sup>360</sup> can be used as part of a training program but also be used to assess performance of your executives, managers, and supervisors on any effectiveness measures your organization feel are warranted.</p>

<h3 class='sln-subheading'>About This Report</h3>

<p>Your report will be generated electronically using The Strategic Leader&rsquo;s software systems when you have completed your analysis. </p>

<p id='interpretation-5' class='interpretation'>%INTERPRETATION_5%</p>

<p>Use of the report is restricted to your own uses. It should be noted that we understand that the content of the report is sensitive and confidential, and every effort is made to ensure its use and storage accordingly.</p>

<p>The report herein is generated from the results of a self-report questionnaire called the SLQ&#8482; completed by you, your direct reports, and your organizational superior. The results conveyed are valid and relevant for career and performance success to the extent that you answered the questions honestly, forthrightly, and to the best of your ability.</p>

<p>Table 5 provides you with a description of each of the leader influence actions to guide your understanding of your results.</p>

<p><h4>Table 5</h4> Description of the Strategic Leader Influence Actions found in the SLQ<sup>360</sup></p>


<div><table class="sln-skills-table">
<caption></caption>
 <thead><tr><th>Strategic Leader Influence Actions</th><th>Description</th> </tr></thead>
<tbody>
 <tr class="even"><td>Transforming</td><td>Transforming actions are taken to influence direction, actions, and opinions in order to change organizational conditions and culture so that learning and change occur as a normal routine of the organization.</td> </tr>
 <tr class="odd"><td>Managing</td><td>Managing actions are taken to maintain consistency in order that current organizational goals are accomplished efficiently and effectively.</td> </tr>
 <tr class="odd"><td>Bonding</td><td>Bonding actions are taken to ensure that trust is an attribute of the system and not just something developed among individuals in order that followers’ exhibit emotional commitment to the organization’s aspirations and values.</td> </tr>
 <tr class="even"><td>Bridging</td><td>Bridging actions are taken to develop alliances with people of power and influence from outside and inside the organization in order to gain insights, support, and resources.</td> </tr>
 <tr class="odd"><td>Bartering</td><td>Bartering actions are taken to give something in exchange in order to strengthen the effectiveness of relationships and alliance building efforts.</td> </tr>
 <tr class="even"><td>Strategic Leadership Capacity</td><td>Strategic Leadership Capacity is the capability to use multiple leader influence actions as appropriate or necessary for the situation.</td> </tr>
</tbody>
</table>
</div>

<p><small>Source: Pisapia, J. (2009). The Strategic Leader: New tactics for a Globalizing World. Charlotte, NC: Information Age Publishing.</small></p>

<h3 class='sln-subheading sln-newpage sln-compare'>Your Results Compared to the Benchmarks</h3>

<p>The following table compares your score to the scores of the most effective (benchmark) leaders in our database.  We derived these Benchmarks from the scores of leaders on our effectiveness scale (the top 10%). The Benchmark scores are displayed on Table 6 for transforming, managing, bonding, bridging, and bartering.  Tests from our databases (over 3000 participants) indicate that benchmark leaders use the strategic thinking skills significantly different from others in the database.</p>

<p>If you scored at or above the benchmark you possess the ability to use the leader action in the same manner as the most effective leaders. It does not mean that you will get an executive or management position. It means that if you get a position, you will have the leadership skills necessary to be a high performer and advance your career. Since the Benchmark was created from self- reported scores, however, we recommend using this measure in a general way to give you some idea of where your strengths and areas of improvement are found.</p>

<h4>Table 6</h4>
<div><table class="sln-compare-table">
<caption></caption>
 <thead><tr><th>Strategic Leader Actions</th><th>Your Score</th><th>Benchmark Leaders</th><th>Managers</th><th>Executives</th><th>For-Profit</th><th>Non-Profit</th> </tr></thead>
<tbody>
 <tr class="even"><td>Transforming</td><td class="number">%Transforming_SCORE_L%</td><td class="number">4.60</td><td class="number">4.10</td><td class="number">4.40</td><td class="number">4.10</td><td class="number">4.40</td> </tr>
 <tr class="odd"><td>Managing</td><td class="number">%Managing_SCORE_L%</td><td class="number">4.40</td><td class="number">4.40</td><td class="number">3.80</td><td class="number">4.20</td><td class="number">4.40</td> </tr>
 <tr class="odd"><td>Bonding</td><td class="number">%Bonding_SCORE_L%</td><td class="number">4.60</td><td class="number">4.30</td><td class="number">4.20</td><td class="number">3.80</td><td class="number">4.20</td> </tr>
 <tr class="even"><td>Bridging</td><td class="number">%Bridging_SCORE_L%</td><td class="number">4.30</td><td class="number">3.59</td><td class="number">4.50</td><td class="number">4.40</td><td class="number">3.70</td> </tr>
 <tr class="odd"><td>Bartering</td><td class="number">%Bartering_SCORE_L%</td><td class="number">3.80</td><td class="number">3.50</td><td class="number">4.20</td><td class="number">3.25</td><td class="number">3.20</td> </tr>
 <tr class="even"><td>Strategic Leadership Capacity</td><td class="number">%Total_SCORE_L%</td><td class="number">4.34</td><td class="number">4.00</td><td class="number">4.10</td><td class="number">3.95</td><td class="number">3.98</td> </tr>
</tbody>
</table>
</div>

<div class='compare-followup'>
  <h4>Assess these questions:</h4>
  <ol>
    <li>Which leader actions - Managing, Transforming, Bonding, Bridging, Bartering do you use most often?</li>
    <li>What is your action profile?</li>
    <li>How do you compare to the Benchmarks* and rest of the leader comparison groups?</li>
    <li>Do you have a wide repertoire of leader of actions?</li>
  </ol>
  <p><u><strong>Interpretation #1</strong></u>: If the person being rated has command [M=>4] of all five leader influence actions they will be more effective in differing situations.  The less command they have of a leader influence action set the less effective they will be if the situation calls for the use of the action set.  For example, if the situation calls for the person build support from external and internal sources, and their bridging influence action mean is 4 or below they will be less effective than leaders whose bridging influence actions fall higher than a mean of 5.</p>  
  <p><u><strong>Interpretation #2</strong></u>:  The person being rated will only be effective if there is a match between the demands of the situation, their organizational role, and their preferred leader actions [the one(s) you use most often]. For example, if they are an executive level leader and their transforming, bridging, and bonding actions are low they will be less effective than those who score higher on these actions.  If they are a manager and their managing, bonding, and bartering actions are low they will be less effective than those whose mean scores are higher.</p>
  <p>
  *Benchmark scores = Scores of Top 10% of leaders on effectiveness scores.<br/>
  **Scores of all leaders (>7,000) taking the assessment by work group.<br/>
  ***Scores of 4 and higher suggest Command to use the influence action. Scores between 3.1 and 3.9 suggest an average ability to use the influence action. Scores below 3.1 suggest a weak ability to use the skill.
  </p>
</div>

<h3 class='sln-subheading sln-newpage sln-action-map'>Your Action Map</h3>

<p class='sln-action-map-directions'>Figure 1 displays the manager’s strengths and areas for improvement from the perspective of the manager, their staff, and their boss. The areas of strength and improvement are grouped by leader action set and color coded.  The <span class='observers'>Green line</span> is the way your staff views your use of these actions. The <span class='supervisor'>Blue line</span> is the way your supervisor views your use of these actions.  </p>

<div class='sln-project-report-chart'>
    <p><strong>Figure 1.</strong></p>
    <h3>360 Results on Leader Influence Actions</h3>
    <table class='sln-chart-legend'>
      <tr>
        <td class='observers'><p>Leadership = <span class='color'>Green</p></div></td>
        <!-- td class='self'><p>Self = <span class='color'>Red</span></p></td -->
        <td class='supervisor'><p>Observers = <span class='color'>Blue</span></p></td>
      </tr>
    </table>
    <p>%MAP_CHART%</p>

    
    <p>
      <table class="xsln-summary-radar-table">
        <caption></caption>
        <thead><tr><th>Skill</th><th>Observer&nbsp;(%ACTION_TABLE_OBS_CNT%)</th><th>Leadership&nbsp;(%ACTION_TABLE_SUP_CNT%)</th></tr></thead>
        <tbody>
         <tr class="odd"><td class='left'>Transforming</td><td>%Transforming_SCORE_O%</td><td>%Transforming_SCORE_L%</td> </tr>
         <tr class="even"><td class='left'>Managing</td><td>%Managing_SCORE_O%</td><td>%Managing_SCORE_L%</td> </tr>
         <tr class="odd"><td class='left'>Bonding</td><td>%Bonding_SCORE_O%</td><td>%Bonding_SCORE_L%</td> </tr>
         <tr class="even"><td class='left'>Bridging</td><td>%Bridging_SCORE_O%</td><td>%Bridging_SCORE_L%</td> </tr>
         <tr class="odd"><td class='left'>Bartering</td><td>%Bartering_SCORE_O%</td><td>%Bartering_SCORE_L%</td> </tr>
         <tr class="even"><td class='left'>Strategic Leadership Capacity</td><td>%Total_SCORE_O%</td><td>%Total_SCORE_L%</td> </tr>
        </tbody>
      </table>
    </p>
    </br>
</div>

<h3 class='sln-subheading sln-newpage sln-action-results'>Your Results on Individual Leader Influence Actions by Self, Observer, and Supervisor</h3>
<h4>Table 7</h4>

<div><table class="sln-360-project-action-data">
<caption></caption>
 <thead><tr><th>Skill</th><th>Item</th><th>Actions<p class="important"> When I am performing my duties ...</p><p></p></th><th>Observers&nbsp;(%ACTION_TABLE_OBS_CNT%)</th><th>Leadership&nbsp;(%ACTION_TABLE_SUP_CNT%)</th> </tr></thead>
<tbody>
 <tr class="odd"><td>Transforming</td><td class="number">13.</td><td> I encourage staff members to become leaders.</td><td class="number">%OBS_13%</td><td class="number">%SUP_13%</td> </tr>
 <tr class="even"><td>Transforming</td><td class="number">15.</td><td> I promote the commitment of people to our organization’s long-term goals.</td><td class="number">%OBS_15%</td><td class="number">%SUP_15%</td> </tr>
 <tr class="odd"><td>Transforming</td><td class="number">21.</td><td> I work hard to set a shared direction for the organization.</td><td class="number">%OBS_21%</td><td class="number">%SUP_21%</td> </tr>
 <tr class="even"><td>Transforming</td><td class="number">23.</td><td> I help people anticipate opportunities.</td><td class="number">%OBS_23%</td><td class="number">%SUP_23%</td> </tr>
 <tr class="odd"><td>Transforming</td><td class="number">29.</td><td> I work to create a shared vision.</td><td class="number">%OBS_29%</td><td class="number">%SUP_29%</td> </tr>
 <tr class="even"><td>Transforming</td><td class="number">31.</td><td> I let people know what is expected of them.</td><td class="number">%OBS_31%</td><td class="number">%SUP_31%</td> </tr>
 <tr class="odd"><td>Transforming</td><td class="number">32.</td><td> I emphasize the organization’s core values in discussions and decision-making.</td><td class="number">%OBS_32%</td><td class="number">%SUP_32%</td> </tr>
 <tr class="odd"><td>Managing</td><td class="number">2.</td><td> I encourage people to use uniform procedures.</td><td class="number">%OBS_2%</td><td class="number">%SUP_2%</td> </tr>
 <tr class="even"><td>Managing</td><td class="number">5.</td><td> I hold people accountable for results.</td><td class="number">%OBS_5%</td><td class="number">%SUP_5%</td> </tr>
 <tr class="odd"><td>Managing</td><td class="number">8.</td><td> I decide how things are to be done.</td><td class="number">%OBS_8%</td><td class="number">%SUP_8%</td> </tr>
 <tr class="even"><td>Managing</td><td class="number">14.</td><td> I enforce rules and policies.</td><td class="number">%OBS_14%</td><td class="number">%SUP_14%</td> </tr>
 <tr class="odd"><td>Managing</td><td class="number">18.</td><td> I establish work rules.</td><td class="number">%OBS_18%</td><td class="number">%SUP_18%</td> </tr>
 <tr class="even"><td>Managing</td><td class="number">24.</td><td> I hold people responsible for their commitments.</td><td class="number">%OBS_24%</td><td class="number">%SUP_24%</td> </tr>
 <tr class="odd"><td>Managing</td><td class="number">25.</td><td> I monitor the work of others.</td><td class="number">%OBS_25%</td><td class="number">%SUP_25%</td> </tr>
 <tr class="even"><td>Managing</td><td class="number">28.</td><td> I tell people to follow the chain of command.</td><td class="number">%OBS_28%</td><td class="number">%SUP_28%</td> </tr>
 <tr class="even"><td>Bonding</td><td class="number">3.</td><td> I am honest with people.</td><td class="number">%OBS_3%</td><td class="number">%SUP_3%</td> </tr>
 <tr class="odd"><td>Bonding</td><td class="number">6.</td><td> I encourage staff members to do “what is right.”</td><td class="number">%OBS_6%</td><td class="number">%SUP_6%</td> </tr>
 <tr class="even"><td>Bonding</td><td class="number">10.</td><td> I respect and consider staff member opinions.</td><td class="number">%OBS_10%</td><td class="number">%SUP_10%</td> </tr>
 <tr class="odd"><td>Bonding</td><td class="number">11.</td><td> I respect the privacy of people.</td><td class="number">%OBS_11%</td><td class="number">%SUP_11%</td> </tr>
 <tr class="even"><td>Bonding</td><td class="number">17.</td><td> I stand firm on decisions based on principle.</td><td class="number">%OBS_17%</td><td class="number">%SUP_17%</td> </tr>
 <tr class="odd"><td>Bonding</td><td class="number">19.</td><td> I am honest with people.</td><td class="number">%OBS_19%</td><td class="number">%SUP_19%</td> </tr>
 <tr class="even"><td>Bonding</td><td class="number">22.</td><td> I put the interests of the organization before my own.</td><td class="number">%OBS_22%</td><td class="number">%SUP_22%</td> </tr>
 <tr class="odd"><td>Bridging</td><td class="number">4..</td><td> I associate with individuals who have influence.</td><td class="number">%OBS_4%</td><td class="number">%SUP_4%</td> </tr>
 <tr class="even"><td>Bridging</td><td class="number">9.</td><td> I develop alliances with people from inside the organization.</td><td class="number">%OBS_9%</td><td class="number">%SUP_9%</td> </tr>
 <tr class="odd"><td>Bridging</td><td class="number">12.</td><td> I develop alliances with people from outside of the organization.</td><td class="number">%OBS_12%</td><td class="number">%SUP_12%</td> </tr>
 <tr class="even"><td>Bridging</td><td class="number">27.</td><td> I seek access to people who have influence over getting things done.</td><td class="number">%OBS_27%</td><td class="number">%SUP_27%</td> </tr>
 <tr class="odd"><td>Bridging</td><td class="number">30.</td><td> I build relationships with influential people.</td><td class="number">%OBS_30%</td><td class="number">%SUP_30%</td> </tr>
 <tr class="even"><td>Bridging</td><td class="number">34.</td><td> I use my connections and networks to make things happen.</td><td class="number">%OBS_34%</td><td class="number">%SUP_34%</td> </tr>
 <tr class="odd"><td>Bartering</td><td class="number">1.</td><td> I am willing to negotiate with people to get things done.</td><td class="number">%OBS_1%</td><td class="number">%SUP_1%</td> </tr>
 <tr class="even"><td>Bartering</td><td class="number">7.</td><td> I try to preserve relationships by finding compromise solutions.</td><td class="number">%OBS_7%</td><td class="number">%SUP_7%</td> </tr>
 <tr class="odd"><td>Bartering</td><td class="number">16.</td><td> I try to create Win-Win solutions in dealing with people.</td><td class="number">%OBS_16%</td><td class="number">%SUP_16%</td> </tr>
 <tr class="even"><td>Bartering</td><td class="number">20.</td><td> I provide small favors in appreciation for good work.</td><td class="number">%OBS_20%</td><td class="number">%SUP_20%</td> </tr>
 <tr class="odd"><td>Bartering</td><td class="number">26.</td><td> I try to meet people needs as well as organizational objectives.</td><td class="number">%OBS_26%</td><td class="number">%SUP_26%</td> </tr>
 <tr class="even"><td>Bartering</td><td class="number">33.</td><td> I negotiate differences with people.</td><td class="number">%OBS_33%</td><td class="number">%SUP_33%</td> </tr>
 <tr class="odd"><td>Bartering</td><td class="number">35.</td><td> I give and take in equal measure in dealing with people.</td><td class="number">%OBS_35%</td><td class="number">%SUP_35%</td> </tr>
</tbody>
</table>
</div>




<div class = 'action-followup'>
  <p><strong>Question:</strong> Is there a disconnect between the leader being rated and those that are rating them on the leader's use of influence actions?</p>
  <p><strong>Interpretation:</strong> If rater scores differ more than one point then it indicates a disconnect of perceptions. These disconnects should be further investigated to understand the differences.  After due consideration, your professional action plan should include strategies to corrected disconnects. </p>
  <p>Scores of 4 and higher suggest a strong ability to use the skill. Scores between 3.1 and 3.9 suggest an average ability to use the skill. Scores below 3.1 suggest a weak ability to use the skill.</p>
</div>

</br>


<h3 class='sln-subheading sln-newpage sln-learning'>Continuing Your Learning</h3>

<div class='sln-resources'>
  <h3>Free Resources and Resources on Demand from SLQ Researchers</h3>
  <ul>
    <li class='reference'>Pisapia, J. (2009). The Strategic Leader: New Tactics for a Globalizing Society. Charlotte USA: Information Age Publishers</li>
    <li class='reference'>Ellington, L. &amp; Pisapia, J. (2013). The strategic leader: Bringing the habits to life. Charlotte: Information Age Publishers.</li>
    <li class='reference'>Pisapia, J. &amp; Pang, N. S.K. (2013). Influence actions of School Principals in Hong Kong, Mainland China, and the United States: A cross-cultural perspective. School Leadership &amp; Management, 33(1), 1-17.</li>
    <li class='reference'>Pisapia, J. &amp; Pang, N. S. K. (2012). The Influence of Cognitive Complexity on Hong Kong School Leader's Ability to Influence Followers. Paper presented at the World Educational Research, Association (WERA) Focal Meeting, Sidney, Australia December 3, 2012.</li>
    <li class='reference'>Pisapia, J. (2011). Finding the Future and Making It Happen. In S. Verma (Ed). Towards the next orbit. New Delhi: Sage Publishers.</li>
    <li class='reference'>Pisapia, J. &amp; Lin, Y. (2011). <a href="http://www.academia.edu/2157967/Leader_values_and_actions_in_the_Chinese_context" target="new">Leader Values and Actions: An Exploratory Study of School Principals in Mainland China</a>. China's New Frontiers.</li>
    <li class='reference'>Pisapia, J. (2010). <a href="http://thestrategicleader.org/content/principles-strategic-leadership-0" target="new">The Principles of Strategic Leadership</a>. The Chinese Executive Leadership Academy, Pudong (CELAP), Shanghai, China May 20, 2010. (Invited)</li>
    <li class='reference'>Uğurluoğlu, U., Celik, Y., &amp; Pisapia, J. (2010). <a href="http://fau.academia.edu/JohnPisapia/Papers/137658/Strategic_Leader_Actions_of_Healthcare_Managers_in_Turkey" target="new">The Use of Strategic Leader Actions by Hospital Managers in Turkey</a>. American Journal of Business Research. 3(1), 33-52.</li>
    <li class='reference'>Uğurluoğlu, U., (2009). Assessment of Strategic Leadership Characteristics of Hospital Managers. Unpublished dissertation. Hacettepe University, Turkey.</li>
    <li class='reference'>Reyes-Guerra, D. (2009). The Relationship of Strategic Leader Actions and Normative Structures. Unpublished dissertation. Florida Atlantic University.</li>
    <li class='reference'>Pisapia, J. &amp; Reyes-Guerra (2008). The Strategic Leader Questionnaire v3 (SLQ). Florida Atlantic University, Boca Raton, Fl. Current &copy; 2009 revisions.</li>
    <li class='reference'>Pisapia, J. (2006). A New Direction for Leadership. (Education Policy Studies Series No. 61). Hong Kong: The Faculty of Education and the Hong Kong Institute of Educational Research. (Monograph)</li>
    <li class='reference'>Pisapia, J. (2006). Mastering Change in a Globalized World. In P. Singh, J. Bhatnagar, &amp; A. Bhandarker (Eds.). Future of work: Mastering change. Chapter 19, pp. 303-327. New Delhi: Excel Books. IBSN: 81-7446-302-2.</li>
    <li class='reference'>Uğurluoğlu, U. (2009). Assessment of Strategic Leadership Characteristics of Hospital Managers. Unpublished dissertation. Hacettepe University, Turkey.</li>
    <li class='reference'>Reyes-Guerra, D. (2009). The Relationship of Strategic Leader Actions and Normative Structures. Unpublished dissertation. Florida Atlantic University.</li>
    <li class='reference'>Yasin, M. (2006). The Use of Strategic Leadership Actions by Deans in Malaysian and American Public Universities. Unpublished dissertation. Florida Atlantic University.</li>
  </ul>
</div>



</div>