<?php

$info = <<<EOT
***************************************************************
解密前请自行备份,因使用此程序造成的数据丢失,作者不承担任何责任.
如遇杀毒软件拦截,请先允许,程序安全性请自行判断,不多解释.
作者:Y.L. QQ:270656184
***************************************************************

请输入一个文件或文件夹路径:
EOT;

if($argc < 2){
	echo $info;
	$file = trim(fgets(STDIN));
}else{
	$file = $argv[1];
}

exec_script($file);
echo "完啦!";

function exec_script($file){
	echo "{$file}\r\n";
	if(is_dir($file)){
		$path = $file;
		//是目录的话,继续
		$dir = dir($path);
		while( $file=$dir->read() ){
			if($file=='.'||$file=='..')continue;
			exec_script($path.'\\'.$file);
		}
		return;
	}elseif(is_file($file)){
		//是文件的话,什么也不做
	}else{
		echo "神马情况? 路径输入正确没有?\r\n再试一次吧!\r\n";
		return;
	}
	
	$result_file = dirname(__FILE__).'/eval_log.txt';
	$bak_file = $file.'.xz.decode.bak';
	@unlink($result_file);
	
	$file = realpath($file);
	exec('php.exe '.escapeshellarg($file));
	
	if(file_exists($result_file)){
		rename($file,$bak_file);
		$content = file_get_contents($result_file);
		$content = explode("\r\n----------------------------------\r\n\r\n",$content);
		$content = array_pop($content);
		if(preg_match('/^\s*\?>(.+)(<\?)?\s*$/s',$content,$match))
			$content = $match[1];
		else
			$content = "<?php\r\n".$content."\r\n?>";
		file_put_contents($file,$content);
		unlink($result_file);
	}
}
