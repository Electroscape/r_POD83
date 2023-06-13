# r_POD83

Software used in the POD83

It is recommended to pull the code directly from git on the pi. So whenever there is an update on git the main branch it is easily deployed on the pi.

Configure the username and email address. If the remote-url is https:// and you are only going to fetch and pull, then there is no actual authentication required. Even typing trash will be accepted.
`git config --global user.name "pod-a-arb"`  <= you can give any name, it will not appear anywhere unless you want to push with that pi
`git config --global user.email "abdullahsaei@hotmail.com"` <= you can write anything “abc@example.test”,  it will not be used unless you want to push with that pi.

Then setup the directory:
1. `cd` to go to home dir
2. `mkdir Electroscape` if the folder does not exist
3. `git clone https://github.com/Electroscape/r_POD83.git`  this will checkout to the main branch directly. You need to use the HTTPS link to overcome the authentication. Otherwise you need to setup SSH keys and a lot of unnecessary stuff.
4. `cd` into the desired module and `bash install.sh`
5. Now change the `config.json` and the `run.sh`, download the videos from drive (for ter 1 and the tv for example) and do all the necessary modification.

If there is any update later only one line is sufficient `git pull`
Other useful commands `git fetch` to check the changes without pulling, `git status` to know the modified files and the branch name and `git checkout <branch name>`  to switch to another branch.

One useful command as well is `git remote set-url origin https://github.com/Electroscape/r_POD83.git` to switch the remote-url to HTTPS  for example if it is previously was set to SSH key  or the other way around.


# Configuration files

 * m_dispenser: /include/header HH parameter
 * m_lablight: /include/header HH paramter
 * m_terminal:
   * run.sh to configure for terminal excection
   * ip_config.json for Kamera credentials and Terminal Ips
   * /static/json/ver_config.json to configure terminal content depending on location
   * /staitc/media insert content for "media file database"
 * pi_tvs: ip_config.json 