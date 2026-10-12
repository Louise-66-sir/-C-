import fs from 'node:fs/promises';
import path from 'node:path';
import crypto from 'node:crypto';
import { fileURLToPath } from 'node:url';

const root = fileURLToPath(new URL('../', import.meta.url));
const siteBuild = true;
const sourceDir = root;
const output = root;
await fs.mkdir(output,{recursive:true});
const raw = (await fs.readFile(path.join(sourceDir,'notes.md'),'utf8')).replace(/<!-- module: \w+ -->\n?/g,'');
const sections = raw.split(/(?=^## \d{2} )/m);
const cover = sections.shift().trim();
const chapters = sections.map((source,index) => {
  const lines = source.trim().split('\n');
  const title = lines.shift().replace(/^## /,'');
  const body = lines.join('\n').trim();
  const topics = body.split(/(?=^### )/m).filter(x=>x.trim()).map(part=> {
    const ls = part.trim().split('\n');
    const title = ls.shift().replace(/^### /,'');
    const id = 's' + title.match(/^\d{2}\.\d+/)[0].replace('.','-');
    return {title,id,body:ls.join('\n').trim()};
  });
  return {title,id:`ch${String(index).padStart(2,'0')}`,topics};
});
const toc = '## 目录\n\n' + chapters.map(c=>`- [${c.title}](#${c.id})`).join('\n');
const cells = [];
const addCell = source => cells.push({cell_type:'markdown',id:crypto.createHash('sha256').update(source).digest('hex').slice(0,12),metadata:{},source:source.match(/.*(?:\n|$)/g).filter(Boolean)});
addCell(cover + '\n\n**格式：阅读型 Notebook · 所有算法使用 C11 · 不需要 C 内核**\n');
addCell(toc+'\n');
for (const c of chapters) {
  addCell(`<a id="${c.id}"></a>\n\n## ${c.title}\n`);
  for (const t of c.topics) {
    const parts = t.body.split(/(```[\s\S]*?```)/g).filter(x=>x.trim());
    let first = true;
    for (const part of parts) {
      addCell((first ? `<a id="${t.id}"></a>\n\n### ${t.title}\n\n` : '')+part.trim()+'\n'); first=false;
    }
  }
}
const notebook = {cells,metadata:{title:'408 数据结构 C 代码复习笔记',language_info:{name:'c',file_extension:'.c',mimetype:'text/x-csrc'},authors:[{name:'Codex'}],ds408:{mode:'reading',standard:'C11',created:'2026-10-12',note:'C examples are fenced Markdown blocks; no kernel required. Standalone compilable sources provided separately.'}},nbformat:4,nbformat_minor:5};
const ipynbName = siteBuild ? 'ds408-notes.ipynb' : '408数据结构_C代码完整复习.ipynb';
const sourcesName = siteBuild ? 'ds408-c-sources.zip' : '408数据结构_C源码与自测.zip';
await fs.writeFile(path.join(output,ipynbName),JSON.stringify(notebook,null,2)+'\n','utf8');

function esc(s) { return s.replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;').replace(/"/g,'&quot;'); }
function inline(s) {
  const stash = [];
  s=s.replace(/`([^`]+)`/g,(_,code)=>{stash.push('<code>'+esc(code)+'</code>');return '\u0001'+(stash.length-1)+'\u0001';});
  s=esc(s).replace(/\[([^\]]+)\]\(([^)]+)\)/g,(_,label,url)=>`<a href="${url}"${url.startsWith('http')?' target="_blank" rel="noopener"':''}>${label}</a>`);
  s=s.replace(/\*\*([^*]+)\*\*/g,'<strong>$1</strong>');
  return s.replace(/\u0001(\d+)\u0001/g,(_,i)=>stash[Number(i)]);
}
function highlight(code) {
  const re=/\/\*[\s\S]*?\*\/|\/\/[^\n]*|"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*'|^\s*#.*$|\b(?:typedef|struct|enum|int|long|char|bool|void|const|static|size_t|return|if|else|for|while|break|continue|true|false|NULL|switch|case|default|unsigned|sizeof)\b|\b\d+\b/gm;
  let html='', last=0;
  for (const m of code.matchAll(re)) {
    html+=esc(code.slice(last,m.index));
    const token=m[0], cls=token.startsWith('//')||token.startsWith('/*')?'comment':/^\s*#/.test(token)?'preproc':token[0]==='"'||token[0]==="'"?'string':/^\d/.test(token)?'number':'keyword';
    html+=`<span class="${cls}">${esc(token)}</span>`; last=m.index+token.length;
  }
  return html+esc(code.slice(last));
}
let blockId=0;
function render(md) {
  const lines=md.split('\n'); let out='',i=0;
  while(i<lines.length) {
    let line=lines[i];
    if(!line.trim()){i++;continue;}
    if(line.startsWith('```')) {
      const lang=line.slice(3).trim(); const code=[]; i++;
      while(i<lines.length&&!lines[i].startsWith('```')) code.push(lines[i++]); i++;
      const id=`code-${++blockId}`;
      out+=`<div class="codebox"><div class="codebar"><span>${lang==='c'?'C11 · 教学代码':'过程示意'}</span><button type="button" class="copy" data-code="${id}" aria-label="复制这段代码">复制代码</button></div><pre><code id="${id}">${lang==='c'?highlight(code.join('\n')):esc(code.join('\n'))}</code></pre></div>`;continue;
    }
    if(/^\|/.test(line)&&i+1<lines.length&&/^\|[ :\- |]+\|\s*$/.test(lines[i+1])) {
      const parse=l=>l.trim().replace(/^\||\|$/g,'').split('|').map(x=>x.trim());
      const header=parse(line); i+=2;const rows=[];
      while(i<lines.length&&/^\|/.test(lines[i])) rows.push(parse(lines[i++]));
      out+='<div class="table-wrap"><table><thead><tr>'+header.map(x=>'<th scope="col">'+inline(x)+'</th>').join('')+'</tr></thead><tbody>'+rows.map(r=>'<tr>'+r.map(x=>'<td>'+inline(x)+'</td>').join('')+'</tr>').join('')+'</tbody></table></div>';continue;
    }
    if(/^[-] /.test(line)||/^\d+\. /.test(line)) {
      const ordered=/^\d/.test(line), tag=ordered?'ol':'ul', re=ordered?/^\d+\. /:/^- /;
      const items=[];while(i<lines.length&&re.test(lines[i]))items.push(lines[i++].replace(re,''));
      out+=`<${tag}>`+items.map(x=>'<li>'+inline(x)+'</li>').join('')+`</${tag}>`;continue;
    }
    if(/^#{1,6} /.test(line)) { const n=line.match(/^#+/)[0].length;out+=`<h${n}>${inline(line.slice(n+1))}</h${n}>`;i++;continue; }
    const para=[];
    while(i<lines.length&&lines[i].trim()&&!/^(?:```|\||[-] |\d+\. |#{1,6} )/.test(lines[i]))para.push(lines[i++]);
    if(!para.length){out+='<p>'+inline(lines[i++])+'</p>';continue;}
    const txt=para.join(' ');
    if(txt.startsWith('**答案：**'))out+='<details class="answer"><summary>查看参考答案</summary><p>'+inline(txt)+'</p></details>';
    else out+='<p>'+inline(txt)+'</p>';
  }
  return out;
}
const topicCount=chapters.reduce((s,c)=>s+c.topics.length,0);
const body=chapters.map(c=>`<section class="chapter" id="${c.id}" aria-labelledby="title-${c.id}"><div class="chapter-heading"><span>CHAPTER ${c.title.slice(0,2)}</span><h2 id="title-${c.id}">${esc(c.title.slice(3))}</h2><div class="chapter-subnav">${c.topics.map(t=>`<a href="#${t.id}">${esc(t.title)}</a>`).join('')}</div></div>${c.topics.map(t=>{
  let content=render(t.body);
  if((c.id==='ch08'&&t.id!=='s08-1')||t.title.includes('参考答案')&&t.title.startsWith('01.8'))content=`<details class="answer"><summary>查看思路与参考实现</summary>${content}</details>`;
  return `<article class="topic" id="${t.id}"><h3>${esc(t.title)}</h3>${content}</article>`;
}).join('')}</section>`).join('');
const sidebar=chapters.map(c=>`<a href="#${c.id}" class="nav-link"><span>${c.title.slice(0,2)}</span>${esc(c.title.slice(3))}</a>`).join('');
const html=`<!doctype html>
<html lang="zh-CN"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><meta name="description" content="408 数据结构 C 代码完整复习：考点、实现、复杂度、易错点与综合练习。"><title>408 数据结构 · C 代码完整复习</title>
<style>
:root{--paper:#f7f7f2;--ink:#202e32;--muted:#627074;--line:#dce3dd;--teal:#0d625b;--pale:#e8f1e9;--orange:#bd5e28;--white:#fff;--code:#15272c;--side:258px}
*{box-sizing:border-box}html{scroll-behavior:smooth;scroll-padding-top:28px}body{margin:0;background:var(--paper);color:var(--ink);font-family:"Microsoft YaHei UI","Microsoft YaHei","Segoe UI",sans-serif;font-size:15px;line-height:1.9}button,input{font:inherit}a{color:var(--teal);text-underline-offset:4px}a:hover{color:#ac4f20}button{cursor:pointer}aside{position:fixed;inset:0 auto 0 0;width:var(--side);padding:30px 22px;background:#edf0e8;border-right:1px solid var(--line);overflow:auto}.brand{font:700 38px/1.2 Georgia,serif;letter-spacing:-2px;color:var(--teal)}.brand small{display:block;font:600 12px/1.8 "Microsoft YaHei",sans-serif;letter-spacing:3px;margin-top:8px;color:var(--muted)}.searchbox{margin:25px 0 8px}.searchbox label{display:block;font-size:12px;font-weight:700;margin-bottom:5px}.searchbox input{width:100%;padding:10px 12px;border:1px solid #b8c9bd;border-radius:6px;background:#fff;font-size:13px;outline:none}.searchbox input:focus{border-color:var(--teal);box-shadow:0 0 0 2px #0d625b22}#search-status{min-height:23px;font-size:11px;color:var(--muted)}nav{margin:15px 0}.nav-link{display:flex;gap:10px;align-items:baseline;padding:8px 8px;border-radius:5px;text-decoration:none;font-size:13px;color:#384c4c;line-height:1.6}.nav-link span{font-family:Consolas,monospace;font-size:12px;color:#8a9891}.nav-link:hover,.nav-link.active{background:#dce9dd;color:var(--teal)}.side-tools{margin-top:25px;padding-top:18px;border-top:1px solid #cdd8cc;display:grid;gap:9px}.side-tools button,.side-tools a{border:1px solid #b7c9bc;border-radius:5px;padding:7px 9px;background:transparent;text-decoration:none;text-align:center;font-size:12px;color:var(--teal)}.side-note{font-size:11px;color:var(--muted);line-height:1.8;margin-top:18px}main{margin-left:var(--side);padding:48px 6vw 90px;max-width:1540px}.hero{padding:0 0 38px;border-bottom:1px solid #b6cbbd;margin-bottom:45px}.eyebrow{font-size:12px;letter-spacing:3px;font-weight:700;color:var(--teal)}h1{font-size:44px;line-height:1.35;letter-spacing:-1.5px;margin:20px 0 18px;font-weight:750}h1 em{font-style:normal;color:var(--teal)}.deck{font-size:17px;color:var(--muted);max-width:720px}.pills{display:flex;flex-wrap:wrap;gap:8px;margin:24px 0}.pills span{font-size:12px;padding:4px 12px;border:1px solid #c4d5c7;border-radius:30px;color:var(--teal)}.stats{display:flex;gap:32px;margin-top:30px}.stats strong{font:700 27px/1.2 Georgia,serif;color:var(--teal);display:block}.stats small{font-size:11px;color:var(--muted)}.hero-note{padding:15px 18px;background:var(--pale);border-left:3px solid var(--teal);font-size:13px;margin-top:24px}.chapter{margin-bottom:52px}.chapter-heading{border-top:2px solid var(--teal);padding-top:18px;margin-bottom:28px}.chapter-heading>span{font-size:11px;letter-spacing:2px;color:var(--teal);font-weight:700}h2{font-size:29px;line-height:1.4;margin:7px 0 17px}.chapter-subnav{display:flex;flex-wrap:wrap;gap:6px 16px}.chapter-subnav a{font-size:11px;text-decoration:none;color:var(--muted)}.chapter-subnav a:hover{text-decoration:underline}.topic{margin:28px 0 35px;scroll-margin-top:30px}.topic h3{font-size:19px;line-height:1.55;margin:0 0 15px;color:var(--ink)}p{margin:12px 0 16px}strong{font-weight:700;color:#174b48}ul,ol{padding-left:25px;margin:14px 0 20px}li{padding-left:3px;margin:5px 0}code{font-family:Consolas,"Cascadia Code","Microsoft YaHei",monospace;font-size:.88em;background:#e8ede8;color:#20554e;padding:2px 5px;border-radius:3px;overflow-wrap:anywhere}.table-wrap{width:100%;overflow:auto;margin:20px 0 25px;border:1px solid var(--line);border-radius:7px;background:#fff}table{border-collapse:collapse;width:100%;font-size:12px;line-height:1.7}th,td{padding:12px 14px;text-align:left;vertical-align:top;border-bottom:1px solid #e6ebe4;min-width:82px}th{font-weight:700;background:#e8eee5;color:#22544b;white-space:nowrap}tr:last-child td{border-bottom:0}tbody tr:nth-child(even){background:#fafbf8}.codebox{margin:22px 0 25px;background:var(--code);border-radius:8px;overflow:hidden;box-shadow:0 3px 10px #15272c0b}.codebar{display:flex;align-items:center;justify-content:space-between;padding:8px 17px;background:#1d3439;color:#9bbcb6;font-size:11px;letter-spacing:.4px}.copy{border:1px solid #4e6c6c;color:#cce4de;border-radius:4px;background:transparent;padding:3px 9px;font-size:11px}.copy:hover{background:#37534f}.codebox pre{margin:0;padding:19px 21px 23px;overflow:auto;tab-size:4}.codebox code{padding:0;background:transparent;color:#dbe8e4;font:12.5px/1.8 Consolas,"Cascadia Code","Microsoft YaHei",monospace;white-space:pre;overflow-wrap:normal}.comment{color:#8aa29c}.keyword{color:#e5b987}.string{color:#b6dca5}.number{color:#e0cc87}.preproc{color:#80c7c3}.answer{border:1px solid #c8d8ca;background:#f0f5ed;border-radius:6px;margin:17px 0;padding:0 16px}.answer summary{cursor:pointer;font-size:13px;font-weight:700;color:var(--teal);padding:12px 0}.answer[open] summary{border-bottom:1px solid #d4dfd2;margin-bottom:14px}.answer .codebox{margin-left:0;margin-right:0}footer{font-size:12px;color:var(--muted);border-top:1px solid var(--line);padding:22px 0}#no-results{padding:40px 0;color:var(--muted)}[hidden]{display:none!important}.top-link{position:fixed;right:22px;bottom:18px;border:1px solid #bed0c1;background:#f7f7f2e8;border-radius:30px;padding:8px 14px;text-decoration:none;font-size:12px}
@media(min-width:1600px){main{padding-left:90px;padding-right:90px}}@media(max-width:1000px){:root{--side:218px}aside{padding:24px 16px}main{padding:35px 4vw}h1{font-size:36px}.nav-link{font-size:12px}}@media(max-width:760px){aside{position:static;width:100%;padding:18px 22px;border-right:0;border-bottom:1px solid var(--line)}.brand{font-size:29px}.brand small{display:inline;margin-left:17px;font-size:10px}.searchbox{margin:16px 0 4px}nav{display:flex;flex-wrap:wrap;gap:4px;margin:10px 0}.nav-link{padding:5px 7px;font-size:11px}.side-tools{display:flex;margin:12px 0 0;padding:12px 0 0}.side-tools a,.side-tools button{flex:1}.side-note{display:none}main{margin:0;padding:30px 22px 60px}h1{font-size:34px}.deck{font-size:15px}.stats{gap:23px}.topic h3{font-size:17px}.codebox pre{padding:16px;font-size:11px}h2{font-size:25px}}
@media print{body{font-size:10pt;line-height:1.6;background:white}aside,.copy,.top-link,.chapter-subnav{display:none!important}main{margin:0;padding:0;max-width:none}h1{font-size:27pt}.hero{margin-bottom:20px;padding-bottom:20px}.chapter{break-before:page}.chapter-heading{margin-bottom:15px}h2{font-size:20pt}h3{font-size:13pt;break-after:avoid}p,li{orphans:3;widows:3}.table-wrap{overflow:visible}table{font-size:8pt}th,td{padding:6px 8px}tr{break-inside:avoid}.codebox{background:#f3f5f1!important;color:#111;box-shadow:none;border:1px solid #ddd;overflow:visible}.codebar{background:#e6ede6;color:#333}.codebox pre{overflow:visible;white-space:pre-wrap;padding:12px}.codebox code{font-size:8pt;color:#111;white-space:pre-wrap;overflow-wrap:anywhere}.keyword,.preproc{color:#235a56}.comment{color:#65736a}.string,.number{color:#664c22}.answer{background:white}.hero-note{background:#f1f4ef}.stats{gap:25px}a{color:#225b54;text-decoration:none}footer{font-size:8pt}}
</style></head><body id="top"><aside><div class="brand">408<small>数据结构 · C 代码笔记</small></div><div class="searchbox"><label for="search">查找考点</label><input id="search" type="search" placeholder="例：链表 / KMP / dijkstra" autocomplete="off"></div><div id="search-status" aria-live="polite">${topicCount} 个小节，可搜索正文与代码</div><nav aria-label="章节目录">${sidebar}</nav><div class="side-tools"><button id="print" type="button">打印 / 保存 PDF</button><a href="${encodeURI(ipynbName)}" download>下载 Notebook</a></div><div class="side-note">标准 C11 · 离线阅读<br>代码用于理解与手写练习。<br>无需配置 Jupyter 内核。<br>整理日期：2026-10-12</div></aside><main><header class="hero"><div class="eyebrow">EXAM 408 / DATA STRUCTURES</div><h1>数据结构<br><em>C 代码完整复习</em></h1><p class="deck">从存储结构到综合算法题，把考点、代码和推导放在一起。先理解不变量，再记住写法。</p><div class="pills"><span>考点地图</span><span>核心实现</span><span>复杂度与边界</span><span>原创练习与答案</span></div><div class="stats"><div><strong>7</strong><small>大专题 + 数组补充</small></div><div><strong>12</strong><small>道综合算法练习</small></div><div><strong>${topicCount}</strong><small>个可检索小节</small></div></div><div class="hero-note"><strong>阅读优先：</strong>所有算法均以 C 编写。此版可直接用浏览器阅读，Notebook 同样不需要 C 内核。<br>另附可编译源码与自测；高级结构的完整实现范围见 09.3。</div></header><div id="no-results" hidden>没有匹配的小节。可以换一个函数名或更短的关键词。</div>${body}<footer>408 数据结构 C 代码复习笔记 · 代码与练习原创编写 · 未指定考试年份 · 公开课程参考链接见 09.4。</footer></main><a href="#top" class="top-link">回到顶部 ↑</a>
<script>
const search=document.querySelector('#search');const topics=[...document.querySelectorAll('.topic')];const chapters=[...document.querySelectorAll('.chapter')];const allDetails=[...document.querySelectorAll('details')];const originalDetails=allDetails.map(d=>d.open);const total=${topicCount};
const hay=topics.map(t=>t.textContent.toLocaleLowerCase());
function filter(){const q=search.value.trim().toLocaleLowerCase();let count=0;topics.forEach((t,i)=>{t.hidden=q&&!hay[i].includes(q);if(!t.hidden)count++;});chapters.forEach(c=>{c.hidden=![...c.querySelectorAll('.topic')].some(t=>!t.hidden);});document.querySelector('.hero').hidden=!!q;document.querySelector('#no-results').hidden=count!==0;document.querySelector('#search-status').textContent=q?'匹配 '+count+' / '+total+' 个小节':total+' 个小节，可搜索正文与代码';allDetails.forEach((d,i)=>{d.open=q?true:originalDetails[i]});}
search.addEventListener('input',filter);search.addEventListener('keydown',e=>{if(e.key==='Escape'){search.value='';filter();}});
document.addEventListener('click',async e=>{const b=e.target.closest('.copy');if(!b)return;const code=document.getElementById(b.dataset.code).textContent;try{await navigator.clipboard.writeText(code);b.textContent='已复制';}catch{const t=document.createElement('textarea');t.value=code;t.style.position='fixed';t.style.left='-9999px';document.body.append(t);t.select();const ok=document.execCommand('copy');t.remove();b.textContent=ok?'已复制':'请选中代码复制';}setTimeout(()=>b.textContent='复制代码',1600);});
let savedOpen=[];window.addEventListener('beforeprint',()=>{savedOpen=allDetails.map(d=>d.open);allDetails.forEach(d=>d.open=true);});window.addEventListener('afterprint',()=>allDetails.forEach((d,i)=>d.open=savedOpen[i]));document.querySelector('#print').addEventListener('click',()=>{search.value='';filter();window.print();});
const observer=new IntersectionObserver(entries=>{entries.forEach(e=>{if(e.isIntersecting){document.querySelectorAll('.nav-link').forEach(a=>a.classList.toggle('active',a.hash==='#'+e.target.id));}});},{rootMargin:'-5% 0px -80% 0px',threshold:0});chapters.forEach(c=>observer.observe(c));
</script></body></html>`;
const readableHtml = html
  .replace('</style>', `
/* Reading layout: Visual Studio-like C formatting and editor colors. */
body{font-family:"Microsoft YaHei UI","Microsoft YaHei","PingFang SC",-apple-system,"Segoe UI",sans-serif}
.codebox{background:#1e1e1e}.codebar{background:#252526;color:#b8b8b8}
.codebox code{font:14px/1.75 Consolas,"Cascadia Code",Menlo,"PingFang SC",monospace;color:#d4d4d4}
.keyword,.preproc{color:#569cd6}.comment{color:#79a85d}.string{color:#ce9178}.number{color:#b5cea8}
.copy{min-height:32px;border-color:#666;color:#ddd}.copy:hover{background:#3b3b3b}
.side-tools{gap:8px}.searchbox input{font-size:16px}
@media(max-width:900px){aside{position:static;width:100%;padding:18px 22px;border-right:0;border-bottom:1px solid var(--line)}.brand{font-size:29px}.brand small{display:inline;margin-left:17px;font-size:10px}.searchbox{margin:16px 0 4px}nav{display:flex;flex-wrap:wrap;gap:4px;margin:10px 0}.nav-link{padding:7px;font-size:12px}.side-tools{display:flex;flex-wrap:wrap;margin:12px 0 0;padding:12px 0 0}.side-tools a,.side-tools button{flex:1;min-width:95px;padding:10px 8px}.side-note{display:none}main{margin:0;padding:30px 22px 60px}.codebox code{font-size:13.5px}.copy{min-height:38px}.hero{margin-bottom:30px}}
@media print{.codebox{background:#f3f5f1!important}.codebox code{font-size:8pt;color:#111}.keyword,.preproc{color:#235a56}.comment{color:#65736a}.string,.number{color:#664c22}}
</style>`)
  .replace('<a href="'+encodeURI(ipynbName)+'" download>下载 Notebook</a>', '<a href="'+encodeURI(ipynbName)+'" download>下载 Notebook</a><a href="'+encodeURI(sourcesName)+'" download>下载 C 源码</a>')
  .replace('<strong>阅读优先：</strong>', '<strong>Visual Studio 常见代码格式：</strong>4 个空格缩进，大括号另起一行，每条语句独立成行。<br>')
  .replace('标准 C11 · 离线阅读', '标准 C11 · Visual Studio 风格');
await fs.writeFile(path.join(output,siteBuild ? 'index.html' : '408数据结构_C代码阅读版.html'),readableHtml,'utf8');
const ids=cells.map(c=>c.id); if(new Set(ids).size!==ids.length)throw Error('Duplicate notebook cell IDs');
const stat={chapters:chapters.length,topics:topicCount,notebookCells:cells.length,cCodeBlocks:(raw.match(/```c\n/g)||[]).length,htmlCodeBlocks:blockId,characters:raw.length,hanCharacters:(raw.match(/\p{Script=Han}/gu)||[]).length};

console.log(JSON.stringify(stat));
